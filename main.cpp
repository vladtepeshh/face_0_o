#include <locale.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "screen.h"
#include "shape.h"

// тут вывод в мой файл просто чтобы норм смотрелось
std::ofstream fout("C:\\Users\\hp\\Desktop\\otcryt.txt");

// полуокружность
class h_circle : public rectangle, public reflectable {
public:
    h_circle(point a, int rd)
            : rectangle(point(a.x - rd, a.y), point(a.x + rd, a.y + rd * 0.7 + 1)) {}
    void draw();
    void flip_horisontally() {}
    void rotate_right() {}
    void rotate_left() {}
};

void h_circle::draw() {
    int x0 = (sw.x + ne.x) / 2, y0 = vert ? ne.y : sw.y;
    int radius = (ne.x - sw.x) / 2;
    int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while (y >= 0) {
        if (vert) {
            put_point(x0 + x, y0 - y * 0.7);
            put_point(x0 - x, y0 - y * 0.7);
        } else {
            put_point(x0 + x, y0 + y * 0.7);
            put_point(x0 - x, y0 + y * 0.7);
        }
        error = 2 * (delta + y) - 1;
        if (delta < 0 && error <= 0) {
            ++x;
            delta += 2 * x + 1;
            continue;
        }
        error = 2 * (delta - x) - 1;
        if (delta > 0 && error > 0) {
            --y;
            delta += 1 - 2 * y;
            continue;
        }
        ++x;
        delta += 2 * (x - y);
        --y;
    }
}

// присоединение снизу
void down(shape& p, const shape& q) {
    point n = q.south();
    point s = p.north();
    p.move(n.x - s.x, n.y - s.y - 1);
}

// присоединение слева и справва
void left_of(shape& p, const shape& q) {
    point pe = p.east();
    point qw = q.west();
    p.move(qw.x - pe.x - 1, qw.y - pe.y);
}

void right_of(shape& p, const shape& q) {
    point pw = p.west();
    point qe = q.east();
    p.move(qe.x - pw.x + 1, qe.y - pw.y);
}

// параллелограмм 4 фигура
class parallelogram : public rotatable, public reflectable {
    parallelogram(const parallelogram&) = delete;
    parallelogram& operator=(const parallelogram&) = delete;

protected:
    point sw;
    int w, h;
    int skew;

    void get_vertices(point& bl, point& br, point& tl, point& tr) const {
        int s = skew;
        if (hor) s = -s;

        if (state == rotated::no) {
            if (!vert) {
                bl = point(sw.x, sw.y);
                br = point(sw.x + w, sw.y);
                tl = point(sw.x + s, sw.y + h);
                tr = point(sw.x + s + w, sw.y + h);
            } else {
                bl = point(sw.x + s, sw.y);
                br = point(sw.x + s + w, sw.y);
                tl = point(sw.x, sw.y + h);
                tr = point(sw.x + w, sw.y + h);
            }
        } else if (state == rotated::right) {
            bl = point(sw.x, sw.y);
            br = point(sw.x + h, sw.y);
            tl = point(sw.x + s, sw.y + w);
            tr = point(sw.x + s + h, sw.y + w);
        } else {
            bl = point(sw.x, sw.y);
            br = point(sw.x + h, sw.y);
            tl = point(sw.x - s, sw.y + w);
            tr = point(sw.x - s + h, sw.y + w);
        }
    }

    void get_bounds(int& left, int& right, int& bottom, int& top) const {
        point bl, br, tl, tr;
        get_vertices(bl, br, tl, tr);
        left   = std::min({bl.x, br.x, tl.x, tr.x});
        right  = std::max({bl.x, br.x, tl.x, tr.x});
        bottom = std::min({bl.y, br.y, tl.y, tr.y});
        top    = std::max({bl.y, br.y, tl.y, tr.y});
    }

public:
    parallelogram(point a, int width, int height, int sk)
            : sw(a), w(width), h(height), skew(sk) {}

    point north() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point((l + r) / 2, t);
    }
    point south() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point((l + r) / 2, b);
    }
    point east() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point(r, (b + t) / 2);
    }
    point west() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point(l, (b + t) / 2);
    }
    point neast() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point(r, t);
    }
    point seast() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point(r, b);
    }
    point nwest() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point(l, t);
    }
    point swest() const override {
        int l, r, b, t;
        get_bounds(l, r, b, t);
        return point(l, b);
    }

    void move(int a, int b) override {
        sw.x += a;
        sw.y += b;
    }

    void resize(double d) override {
        w    = static_cast<int>(w * d);
        h    = static_cast<int>(h * d);
        skew = static_cast<int>(skew * d);
    }

    void draw() override {
        point bl, br, tl, tr;
        get_vertices(bl, br, tl, tr);
        put_line(bl, br);
        put_line(tl, tr);
        put_line(bl, tl);
        put_line(br, tr);
    }
};

// еблет этого гнома как в примере
class myshape : public rectangle {
    int w, h;
    line l_eye;
    line r_eye;
    line mouth;

public:
    myshape(point, point);
    void draw();
    void move(int, int);
    void resize(double r) {
        rectangle::resize(r);
        rectangle::move(w * (1 - r) * 0.5, h * (1 - r) * 0.5);
    }
    void rotate_left() {}
    void rotate_right() {}
};

myshape::myshape(point a, point b)
        : rectangle(a, b),
          w(neast().x - swest().x + 1),
          h(neast().y - swest().y + 1),
          l_eye(point(swest().x + 2, swest().y + h * 3 / 4), 2),
          r_eye(point(swest().x + w - 4, swest().y + h * 3 / 4), 2),
          mouth(point(swest().x + 2, swest().y + h / 4), w - 4)
{
}

void myshape::draw() {
    rectangle::draw();
    int a = (swest().x + neast().x) / 2;
    int b = (swest().y + neast().y) / 2;
    put_point(point(a, b));
}

void myshape::move(int a, int b) {
    rectangle::move(a, b);
    l_eye.move(a, b);
    r_eye.move(a, b);
    mouth.move(a, b);
}

// тут выводим и в консось и в файл одновременно
void shape_refresh_to_file() {
    screen_clear();
    for (auto p : shape::shapes)
        p->draw();

    for (int y = YMAX - 1; 0 <= y; --y) {
        for (auto x : screen[y]) {
            std::cout << x;
            fout << x;
        }
        std::cout << '\n';
        fout << '\n';
    }
}

int main() {
    setlocale(LC_ALL, "Rus");
    screen_init();

    // объявим набор фигур
    rectangle hat(point(0, 0), point(14, 5));
    line brim(point(20, 9), 17);
    myshape face(point(15, 10), point(27, 18));
    h_circle beard(point(40, 10), 5);

    // три параллелограмма для позиций 2, 3, 14
    parallelogram sideburn_left(point(50, 0), 4, 7, 2);
    parallelogram sideburn_right(point(60, 0), 4, 7, 2);
    parallelogram spike(point(70, 0), 4, 7, 2);

    shape_refresh_to_file();
    std::cout << "~~~ Generated... ~~~\n";
    fout << "~~~ Generated... ~~~\n";
    std::cin.get();

    // подготовка к сборке
    hat.rotate_right();
    brim.resize(2.0);
    face.resize(1.2);
    beard.flip_vertically();
    beard.resize(1.2);

    // меняем параллелограммы
    sideburn_left.flip_horisontally();
    sideburn_left.resize(1.2);
    sideburn_right.resize(1.2);
    spike.flip_vertically();
    spike.resize(1.2);

    shape_refresh_to_file();
    std::cout << "~~~ Prepared... ~~~\n";
    fout << "~~~ Prepared... ~~~\n";
    std::cin.get();

    // собираем изображение
    up(brim, face);
    up(hat, brim);
    down(beard, face);

    // размещение параллелограммов
    left_of(sideburn_left, face);
    right_of(sideburn_right, face);
    up(spike, hat);

    shape_refresh_to_file();
    std::cout << "~~~ Ready! ~~~\n";
    fout << "~~~ Ready! ~~~\n";
    std::cin.get();

    fout.close();
    screen_destroy();
    return 0;
}
