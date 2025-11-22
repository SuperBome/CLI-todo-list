pkgname=todo_ornot_todo
pkgver=1.0
pkgrel=1
pkgdesc="simple todo list"
arch=('x86_64')
license=('custom')
depends=()
source=('https://raw.githubusercontent.com/SuperBome/CLI-todo-list/refs/heads/main/todo_ornot_todo.cpp'
        'https://raw.githubusercontent.com/SuperBome/CLI-todo-list/refs/heads/main/todolist.txt')
md5sums=('1bb6c58ab5dbca279cc6e6fa305feed5'
         '56fb04f042985c3487fd0d07731058af')

build()
{
    cd "$srcdir"
    g++ todo_ornot_todo.cpp -o td_on_td
}

package()
{
    install -Dm755 "$srcdir/td_on_td" "$pkgdir/usr/bin/td_on_td"

    install -Dm644 "$srcdir/todolist.txt" "$pkgdir/usr/share/todo_ornot_todo/todolist.txt"
}
