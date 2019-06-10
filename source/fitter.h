#ifndef LC_FITTER
#define LC_FITTER

/*
=======================================
|	General		C++						|
|	Authors:	Luca Ciucci				|
|				Sara Bougrine			|
|				Serena Bruzzesi			|
|	Version:	0						|
|	Date:		x / x / 2019			|
|	e-mail:		develop@lucaciucci99.com|
|	licese:		license.md				|
=======================================
*/

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>      // std::ifstream
#include "LC/LC.h"
#include <complex>

using namespace lc::math;
double fabs(std::complex<double> c)
{
	return sqrt(c.real() * c.real() + c.imag() * c.imag());
}

double sqr(double x)
{
	return x * x;
}

#define WINSIZEX 800
#define WINSIZEY 800
std::vector<Vector3<>> out_dft_pts;
sf::RenderWindow window_dft(sf::VideoMode(WINSIZEX * 2, WINSIZEY), "dft (esperienza 12) @ Luca Ciucci");


double f(double w, double F0, double w0, double gamma)
{
	return F0 / sqrt(sqr(w0 * w0 - w * w) + 4.0 * (gamma * gamma) * (w * w));
}

double chi2(double F0, double w0, double gamma)
{
	double clamp_v = 100.0 / lc::math::m_PI_2<>;
	double sum_v = 0.0;
	for (int i = 0; i < out_dft_pts.size(); i++)
	{
		sum_v += 
		sqr( 
			atan((out_dft_pts[i].y - f(out_dft_pts[i].x, F0, w0, gamma)) / clamp_v)
			* clamp_v 
		);
	}
	return sum_v / out_dft_pts.size();
}

void plot_f(double F0, double w0, double gamma)
{
	//window_dft.clear();

	double max_height = 100;
	sf::CircleShape circle(2.0);
	circle.setOrigin(sf::Vector2f(2.0, 2.0));
	circle.setFillColor(sf::Color::Red);
	for (int i = 0; i < out_dft_pts.size(); i++)
	{
		int x_px = (out_dft_pts[i].x) / (8.0) * window_dft.getSize().x;
		int y_px = (1.0 - out_dft_pts[i].y / max_height) * window_dft.getSize().y;
		circle.setPosition(sf::Vector2f(x_px, y_px));
		window_dft.draw(circle);
	}

	int N = 1000;
	sf::VertexArray lines(sf::LinesStrip, N);
	double w = 0.0;
	for (int i = 0; i < N; i++)
	{
		w += 8.0 / N;
		double A = f(w, F0, w0, gamma);
		lines[i].position.x = w/8.0 * window_dft.getSize().x;
		lines[i].position.y = (1.0 - A / 100) * window_dft.getSize().y;
		lines[i].color = sf::Color::Blue;
	}
	window_dft.draw(lines);
	//window_dft.display();
	sf::Event event;
	while (window_dft.pollEvent(event));
}


class Fitter {
private:
	std::vector<Vector3<>>
		v,// dati
		dft_points;// punti generati dalla dft
	Vector3<> min, max;
	Vector3<> dft_startPos, dft_endPos;
	sf::Event event;

	int dft_start_index, dft_end_index;

	double m, A, dA;
	int speed;

	std::ofstream data_out;
	sf::RenderWindow window;
public:
	Fitter(std::string name, int _speed);
	~Fitter();


	sf::Vector2f vTov2f(Vector3<> v)
	{
		Vector3<> w((v-min).x / (max-min).x, (v - min).y / (max - min).y, 0.0);
		return sf::Vector2f(w.x * window.getSize().x, w.y * window.getSize().y);
	}

	Vector3<> v2fToV(sf::Vector2f v)
	{
		Vector3<> w(v.x / window.getSize().x, v.y / window.getSize().y, 0.0);
		w.x *= (max - min).x;
		w.y *= (max - min).y;
		w += min;
		return w;
	}

	void draw(bool display = true);
	void plot_dft(double t_min, double t_max, double omega_min = 0, double omega_max = 8, double omega_step = 0.01);

	void go(void);

	std::complex<double> dft_i(double q);
	Vector3<> dft_max(double t_min, double t_max, double omega_min = 0, double omega_max = 8, double omega_step = 0.001, double divider = 10, int maxRefinementStep = 1);
	void run_dft(int nSubDiv = 10, double overlapPercentage = 0.5, double time_interval = 10/*sec*/);
	void save_dft(std::string path, double wMin, double wMax, double dw = 0.001);
};


#endif // !LC_FITTER
