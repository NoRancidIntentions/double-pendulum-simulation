#include <SFML/Graphics.hpp>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>

#include <cmath>
#include <optional>
#include <string>

struct Params
{
    double m1, L1, m2, L2;
};

double t1, t2, m1, m2, l1, l2, dt1, dt2;
const double g = 9.81;
const double dt = 0.01;
const double pi = 3.14159;
int WIDTH = 1280;
int HEIGHT = 720;
const int scale = 200;

int system(double /*t*/, const double y[], double dydt[], void *params)
{
    Params *p = static_cast<Params *>(params);
    m1 = p->m1;
    m2 = p->m2;
    l1 = p->L1;
    l2 = p->L2;

    dydt[0] = y[2];
    dydt[1] = y[3];

    const double den = (2 * m1 + m2 - m2 * cos(2 * y[0] - 2 * y[1]));

    dydt[2] = (-g * (2 * m1 + m2) * sin(y[0])
               - m2 * g * sin(y[0] - 2 * y[1])
               - 2 * sin(y[0] - y[1]) * m2 *
                     (y[3] * y[3] * l2 + y[2] * y[2] * l1 *
                      cos(y[0] - y[1]))) /
              (l1 * den);

    dydt[3] = (2 * sin(y[0] - y[1]) *
               (y[2] * y[2] * l1 * (m1 + m2)
                + g * (m1 + m2) * cos(y[0])
                + y[3] * y[3] * l2 * m2 * cos(y[0] - y[1]))) /
              (l2 * den);

    return GSL_SUCCESS;
}

int main()
{
    double t = 0.0;
    // State order: theta_1, theta_2, omega_1, omega_2.
    double y[4] = {pi / 4.0, pi, 0.0, 0.0};
    Params params = {1.0, 1.0, 1.0, 1.0};

    l1 = params.L1;
    l2 = params.L2;

    gsl_odeiv2_system sys = {system, nullptr, 4, &params};
    gsl_odeiv2_driver *d = gsl_odeiv2_driver_alloc_y_new(
        &sys, gsl_odeiv2_step_rk4, dt, 1e-6, 1e-6);
    if (d == nullptr)
    {
        return 1;
    }

    sf::CircleShape pivot(10.0f);
    pivot.setFillColor(sf::Color::White);

    sf::CircleShape bob1(7.0f);
    bob1.setFillColor(sf::Color::White);

    sf::CircleShape bob2(7.0f);
    bob2.setFillColor(sf::Color::White);

    const sf::Vector2f pivotPos(WIDTH / 2.0f, HEIGHT / 4.0f);
    pivot.setOrigin({pivot.getRadius(), pivot.getRadius()});
    bob1.setOrigin({bob1.getRadius(), bob1.getRadius()});
    bob2.setOrigin({bob2.getRadius(), bob2.getRadius()});
    pivot.setPosition(pivotPos);

    bob1.setPosition({
        pivotPos.x + scale * l1 * sin(y[0]),
        pivotPos.y + scale * l1 * cos(y[0])});

    bob2.setPosition({
        pivotPos.x + scale * l1 * sin(y[0]) + scale * l2 * sin(y[1]),
        pivotPos.y + scale * l1 * cos(y[0]) + scale * l2 * cos(y[1])});

    sf::VertexArray rod(sf::PrimitiveType::LineStrip, 3);
    rod[0].position = pivotPos;
    rod[1].position = bob1.getPosition();
    rod[2].position = bob2.getPosition();
    rod[0].color = sf::Color::White;
    rod[1].color = sf::Color::White;
    rod[2].color = sf::Color::White;

    sf::RenderWindow window(
        sf::VideoMode({static_cast<unsigned int>(WIDTH),
                       static_cast<unsigned int>(HEIGHT)}),
        "Double Pendulum");
    window.setFramerateLimit(static_cast<unsigned int>(1.0 / dt));

    sf::Clock clock;
    float fps = 0.0f;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        const double ti = t + dt;

        window.clear();
        window.draw(pivot);
        window.draw(bob1);
        window.draw(bob2);
        window.draw(rod);
        window.display();

        fps = 1.0f / clock.restart().asSeconds();
        window.setTitle(std::to_string(fps));

        if (gsl_odeiv2_driver_apply(d, &t, ti, y) != GSL_SUCCESS)
        {
            break;
        }

        bob1.setPosition({
            pivotPos.x + scale * l1 * sin(y[0]),
            pivotPos.y + scale * l1 * cos(y[0])});

        bob2.setPosition({
            pivotPos.x + scale * l1 * sin(y[0]) + scale * l2 * sin(y[1]),
            pivotPos.y + scale * l1 * cos(y[0]) + scale * l2 * cos(y[1])});

        rod[1].position = bob1.getPosition();
        rod[2].position = bob2.getPosition();

        const std::string time = "Time: " + std::to_string(t);
        const std::string angle1 = "Angle 1: " + std::to_string(y[0] * 180.0 / pi);
        const std::string angle2 = "Angle 2: " + std::to_string(y[1] * 180.0 / pi);
        window.setTitle(time + " " + angle1 + " " + angle2);
    }

    gsl_odeiv2_driver_free(d);
    return 0;
}
