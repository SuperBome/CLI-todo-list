#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <filesystem>

using namespace std;

/*
 * Controlla se una stringa contiene almeno una lettera.
 * Serve per evitare task vuote o fatte solo di numeri.
 */
bool contieneLettera(const string& s) {
    for (char c : s) {
        if (isalpha(c)) return true;
    }
    return false;
}

/*
 * Controlla che una stringa sia formata solo da cifre.
 * Serve per validare l'ID inserito dall'utente.
 */
bool soloNumeri(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

/*
 * Struttura dati per una task.
 * Viene mantenuto:
 * - id: indice numerico
 * - name: testo della task
 * - status: 0 non fatta, 1 fatta
 */
class Task {
public:
    int id;
    string name;
    int status;

    Task(int tid, const string& tname)
    : id(tid), name(tname), status(0) {}

    void complete() { status = 1; }
    void empty() { status = 0; }
    void modify(const string& nuovoNome) { name = nuovoNome; }
};

/*
 * Allinea gli ID dopo una rimozione per evitare buchi nella lista.
 * Ogni task con ID maggiore dell'eliminato viene scalata di 1.
 */
void fixIDs(vector<Task>& tasks, int deletedID) {
    for (auto& t : tasks) {
        if (t.id > deletedID)
            t.id--;
    }
}

int main() {
    system("clear");

    vector<Task> tasks;

    /*
     * Creazione del percorso ~/.todo-list/todolist.txt
     * La directory viene creata se non esiste.
     */
    const char* homeDir = getenv("HOME");
    string folderPath = string(homeDir) + "/todo-list";
    string filePath = folderPath + "/todolist.txt";

    filesystem::create_directories(folderPath);

    /*
     * FORMAT DEL FILE (stabile):
     * id;status;name
     *
     * Esempio:
     * 1;0;fare la spesa
     * 2;1;studiare
     */

    // Caricamento della lista da disco
    ifstream lista(filePath);
    if (lista.is_open()) {
        string riga;

        while (getline(lista, riga)) {
            if (riga.empty()) continue;

            stringstream ss(riga);
            string id_s, status_s, name;

            // parsing separato da delimitatore ';'
            getline(ss, id_s, ';');
            getline(ss, status_s, ';');
            getline(ss, name, ';');

            // se i campi obbligatori sono vuoti la riga viene ignorata
            if (id_s.empty() || status_s.empty()) continue;

            int id = stoi(id_s);
            int status = stoi(status_s);

            Task t(id, name);
            t.status = status;
            tasks.push_back(t);
        }

        lista.close();
    }

    /*
     * Funzione per salvare su file nel formato corretto.
     * Ogni salvataggio sovrascrive l'intero file.
     */
    auto salvaLista = [&]() {
        ofstream out(filePath, ios::trunc);
        for (const auto& t : tasks) {
            out << t.id << ";" << t.status << ";" << t.name << "\n";
        }
        out.close();
    };

    bool trovata = false;

    while (true) {
        cout << "\n";

        /*
         * Stampa della lista:
         * id [ ] testo
         * id [x] testo
         */
        if (tasks.empty())
            cout << "Niente da fare lesgo\n\n";
        else {
            for (const auto& t : tasks)
                cout << t.id << " [" << (t.status == 1 ? "x" : " ") << "] " << t.name << "\n";
        }

        // Input dell'utente
        cout << "> ";
        string input;
        getline(cin, input);
        if (input.empty()) continue;

        string cmd, arg;
        size_t spazio = input.find(' ');

        system("clear");

        /*
         * Comandi senza argomento:
         * h → help
         */
        if (spazio == string::npos) {
            cmd = input.substr(0);

            if (cmd == "h") {
                cout << "\t -- LISTA COMANDI --\n"
                << "a <nome> - Aggiunta di una task\n"
                << "m <ID> <nuovoNome> - Modifica task\n"
                << "r <ID> - Rimuove una task\n"
                << "c <ID> - Spunta una task\n"
                << "e <ID> - Svuota la check di una task\n";
            } else {
                cout << "OCCHIO: comando non valido o senza argomento, premi \"h\" per i comandi possibili\n";
            }
            continue;
        }

        /*
         * Separazione comando + argomento
         */
        cmd = input.substr(0, spazio);
        arg = input.substr(spazio + 1);

        /*
         * --- ADD TASK ---
         */
        if (cmd == "a") {
            if (!contieneLettera(arg)) {
                cout << "OCCHIO: l'argomento deve contenere almeno una lettera\n";
                continue;
            }

            int nuovoId = tasks.empty() ? 1 : tasks.back().id + 1;
            Task t(nuovoId, arg);
            tasks.push_back(t);

            cout << "Aggiungo la task: " << arg << "\n";

            salvaLista();
        }

        /*
         * --- MODIFY TASK ---
         */
        else if (cmd == "m") {
            size_t spazio2 = arg.find(' ');
            if (spazio2 == string::npos) {
                cout << "OCCHIO: devi fornire ID e nuovo nome della task\n";
                continue;
            }

            string idStr = arg.substr(0, spazio2);
            string nuovoNome = arg.substr(spazio2 + 1);

            if (!soloNumeri(idStr)) {
                cout << "OCCHIO: l'ID deve essere un numero positivo\n";
                continue;
            }

            int id = stoi(idStr);

            if (!contieneLettera(nuovoNome)) {
                cout << "OCCHIO: il nuovo nome deve contenere almeno una lettera\n";
                continue;
            }

            for (auto& t : tasks) {
                if (t.id == id) {
                    t.modify(nuovoNome);
                    trovata = true;
                    break;
                }
            }

            if (trovata) cout << "Modifico la task con ID " << id << " in: " << nuovoNome << "\n";
            else cout << "OCCHIO: l'ID inserito non esiste\n";

            trovata = false;
            salvaLista();
        }

        /*
         * --- REMOVE TASK ---
         */
        else if (cmd == "r") {
            if (!soloNumeri(arg)) {
                cout << "OCCHIO: l'ID deve essere un numero positivo\n";
                continue;
            }

            int id = stoi(arg);

            for (auto it = tasks.begin(); it != tasks.end(); ++it) {
                if (it->id == id) {
                    int deletedID = it->id;
                    tasks.erase(it);
                    fixIDs(tasks, deletedID);
                    cout << "Rimuovo la task con ID: " << id << "\n";
                    trovata = true;
                    break;
                }
            }

            if (!trovata) cout << "OCCHIO: l'ID inserito non esiste\n";

            trovata = false;
            salvaLista();
        }

        /*
         * --- COMPLETE TASK ---
         */
        else if (cmd == "c") {
            if (!soloNumeri(arg)) {
                cout << "OCCHIO: l'ID deve essere un numero positivo\n";
                continue;
            }

            int id = stoi(arg);

            for (auto& t : tasks) {
                if (t.id == id) {
                    t.complete();
                    trovata = true;
                    break;
                }
            }

            if (trovata) cout << "Task completata\n";
            else cout << "OCCHIO: l'ID inserito non esiste\n";

            trovata = false;
            salvaLista();
        }

        /*
         * --- EMPTY TASK ---
         */
        else if (cmd == "e") {
            if (!soloNumeri(arg)) {
                cout << "OCCHIO: l'ID deve essere un numero positivo\n";
                continue;
            }

            int id = stoi(arg);

            for (auto& t : tasks) {
                if (t.id == id) {
                    t.empty();
                    trovata = true;
                    break;
                }
            }

            if (trovata) cout << "Svuoto il check di una task\n";
            else cout << "OCCHIO: l'ID inserito non esiste\n";

            trovata = false;
            salvaLista();
        }

        /*
         * --- UNKNOWN COMMAND ---
         */
        else {
            cout << "OCCHIO: comando non valido, usa \"h\" per conoscere i comandi esistenti\n";
        }
    }

    return 0;
}
