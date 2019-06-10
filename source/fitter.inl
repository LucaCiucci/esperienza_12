#include "fitter.h"

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


Fitter::Fitter(std::string name, int _speed) :
	min(100000.0), max(-1000000.0),
	m(0), A(0.0), dA(0.0), speed(_speed), data_out("data_out.txt", std::ifstream::app),
	window(sf::VideoMode(WINSIZEX, WINSIZEY), "esperienza 12 @ Luca Ciucci")
{
	std::ifstream file(name, std::ifstream::in);

	double x_, t_;

	char c;
	do file.get(c); while (file.good() && c != '\n');
	do file.get(c); while (file.good() && c != '\n');
	do file.get(c); while (file.good() && c != '\n');
	do file.get(c); while (file.good() && c != '\n');

	while (file >> t_ >> x_)
	{
		//std::cout << t_ << " " << x_ << std::endl;
		v.push_back(Vector3<>(t_, x_, 0.0));
	}

	for (int i = 0; i < v.size(); i++)
	{
		if (min.x > v[i].x)
			min.x = v[i].x;
		if (min.y > v[i].y)
			min.y = v[i].y;

		if (max.x < v[i].x)
			max.x = v[i].x;
		if (max.y < v[i].y)
			max.y = v[i].y;
		//max.x = min.x + 60.0;

		m += v[i].y;
	}
	m /= v.size();

	dft_start_index = 0;
	dft_end_index = v.size();

	data_out << "flie " << name << std::endl;
}

Fitter::~Fitter()
{
}

void Fitter::draw(bool display)
{
	window.clear();

	sf::CircleShape circle(2.0);
	sf::Vertex error[2] =
	{
		sf::Vertex(sf::Vector2f(10, 10)),
		sf::Vertex(sf::Vector2f(150, 150))
	};
	error[1].color = error[0].color = sf::Color::Color(sf::Color::White.r, sf::Color::White.g, sf::Color::White.b, 64);
	circle.setOrigin(sf::Vector2f(2.0,2.0));
	circle.setFillColor(sf::Color::White);
	sf::VertexArray t(sf::LinesStrip, v.size());
	for (int i = 0;i < v.size(); i++)
	{
		error[0].position = vTov2f(v[i] + Vector3<>(0.0, + 1.0, 0.0));
		error[1].position = vTov2f(v[i] + Vector3<>(0.0, - 1.0, 0.0));
		window.draw(error, 2, sf::Lines);
		t[i].position = vTov2f(v[i]);
		t[i].color = sf::Color::Green;
		circle.setPosition(vTov2f(v[i]));
		window.draw(circle);
	}
	window.draw(t);



	sf::Vertex verticalLine[2] =
	{
		sf::Vertex(sf::Vector2f(10, -10000)),
		sf::Vertex(sf::Vector2f(150, 10000))
	};
	{// min
		verticalLine[1].position.x =  verticalLine[0].position.x = vTov2f(dft_startPos).x;
		verticalLine[0].color = verticalLine[1].color = sf::Color::Blue;
		window.draw(verticalLine, 2, sf::Lines);
	}
	{// max
		verticalLine[1].position.x = verticalLine[0].position.x = vTov2f(dft_endPos).x;
		verticalLine[0].color = verticalLine[1].color = sf::Color::Red;
		window.draw(verticalLine, 2, sf::Lines);
	}

	if (display)
		window.display();
}

inline void Fitter::plot_dft(double t_min, double t_max, double omega_min, double omega_max, double omega_step)
{
	//window_dft.clear();
	
	double max_height = 100;
	sf::CircleShape circle(2.0);
	circle.setOrigin(sf::Vector2f(2.0, 2.0));
	circle.setFillColor(sf::Color::Red);
	for (int i = 0; i < out_dft_pts.size(); i++)
	{
		int x_px = (out_dft_pts[i].x - omega_min) / (omega_max - omega_min) * window_dft.getSize().x;
		int y_px = (1.0 - out_dft_pts[i].y / max_height) * window_dft.getSize().y;
		circle.setPosition(sf::Vector2f(x_px, y_px));
		window_dft.draw(circle);
	}

	int start_index(0), end_index(v.size());
	for (int i = 0; i < v.size(); i++)
		if (v[i].x >= t_min)
		{
			start_index = i;
			break;
		}
	for (int i = start_index; i < v.size(); i++)
		if (v[i].x >= t_max)
		{
			end_index = i;
			break;
		}
	dft_start_index = start_index; dft_end_index = end_index;

	int N = (omega_max - omega_min) / omega_step;
	sf::VertexArray lines(sf::LinesStrip, N);
	double omega = omega_min;
	for (int i = 0; i < N; i++)
	{
		omega += omega_step;
		double A = fabs(dft_i(omega));
		lines[i].position.x = (omega - omega_min) / (omega_max-omega_min) * window_dft.getSize().x;
		lines[i].position.y = (1.0 - A / max_height) * window_dft.getSize().y;
		lines[i].color = sf::Color::Green;
	}
	window_dft.draw(lines);
	//window_dft.display();
	sf::Event event;
	while (window_dft.pollEvent(event));
}

void Fitter::go(void)
{
#define MULTIPLIER 1.5;
	std::ofstream out_file("data.txt", std::ifstream::app);
	bool flag = true;
	while (window.isOpen() && flag)
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::F:
				{
					run_dft(-10, 0.5);
					break;
				}
				case sf::Keyboard::Right:
				{
					double d = (max.x - min.x) * 0.05;
					min.x += d;
					max.x += d;
					break;
				}
				case sf::Keyboard::Left:
				{
					double d = (max.x - min.x) * 0.05;
					min.x -= d;
					max.x -= d;
					break;
				}
				case sf::Keyboard::Add:
				{
					double c = (min.x + max.x) * 0.5;
					double a = (max.x - min.x) * 0.5;
					min.x = c - a / MULTIPLIER;
					max.x = c + a / MULTIPLIER;
					break;
				}
				case sf::Keyboard::Subtract:
				{
					double c = (min.x + max.x) * 0.5;
					double a = (max.x - min.x) * 0.5;
					min.x = c - a * MULTIPLIER;
					max.x = c + a * MULTIPLIER;
					break;
				}
				case sf::Keyboard::Escape:
					// TODO
				{
					// A è la media delle ampiezze
					out_file << speed << " ";
					for (int i = 0; i < dft_points.size(); i++)
					{
						A += fabs(dft_points[i].y - m);
					}
					A /= dft_points.size();
					double var(0.0);
					for (int i = 0; i < dft_points.size(); i++)
					{
						var += sqr(fabs(dft_points[i].y - m) - A);
					}
					var /= (dft_points.size() - 1);
					dA = sqrt(var) / sqrt(dft_points.size());

					std::cout << A << "+-" << dA << std::endl;
					out_file << A << " " << dA << std::endl;
					out_file.close();
					data_out.close();
					flag = false;
					//window.close();
				}
				break;
				default:
					break;
				}
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				switch (event.mouseButton.button)
				{
				case sf::Mouse::Button::Left:
					dft_startPos = Vector3<>(v2fToV(sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)));
					break;
				case sf::Mouse::Button::Right:
					dft_endPos = Vector3<>(v2fToV(sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)));
					break;
				default:
					break;
				}
			}
		}
		draw();
	}
}

inline std::complex<double> Fitter::dft_i(double q)
{
	std::complex<double> somma = 0.0;
	std::complex<double> i = std::complex<double>(0.0, 1.0);
	double pi = lc::math::m_PI<>;
	int N = dft_end_index - dft_start_index;
	for (int k = dft_start_index; k < dft_end_index; k++)
	{
		somma += (v[k].y - m) * exp(-i * q * v[k].x);
	}
	return somma / (double)N;
}

inline Vector3<> Fitter::dft_max(double t_min, double t_max, double omega_min, double omega_max, double omega_step, double divider, int maxRefinementStep)
{
	window_dft.clear();
	plot_dft(t_min, t_max, omega_min, omega_max);
	window_dft.display();
	double max_omega = -1.0;
	double max_A = -1.0;
	for (int sub_i = 0; sub_i < maxRefinementStep; sub_i++)
	{
		int start_index(0), end_index(v.size());
		for (int i = 0; i < v.size(); i++)
			if (v[i].x >= t_min)
			{
				start_index = i;
				break;
			}
		for (int i = start_index; i < v.size(); i++)
			if (v[i].x >= t_max)
			{
				end_index = i;
				break;
			}
		dft_start_index = start_index; dft_end_index = end_index;
		for (double omega = omega_min; omega < omega_max; omega += omega_step)
		{
			double A = fabs(dft_i(omega));
			if (A > max_A)
			{
				max_A = A;
				max_omega = omega;
			}
		}
	}
	Vector3<> dft_max;
	dft_max.x = max_omega;
	dft_max.y = max_A;
	return dft_max;
}

inline void Fitter::run_dft(int nSubDiv, double overlapPercentage, double time_interval)
{
	double width;

	if (nSubDiv <= 0)
	{
		width = time_interval;
		nSubDiv = ((dft_endPos - dft_startPos).x - width) / width / (1 - overlapPercentage);
	}
	else
	{
		width = (dft_endPos - dft_startPos).x;
		width = width / (1.0 + (1.0 - overlapPercentage) * nSubDiv);
	}

	double step = width * (1 - overlapPercentage);
	double width_px = vTov2f(Vector3<>(width, 0.0, 0.0)).x;
	double t_start = dft_startPos.x;
	double t_end = t_start + width;
	for (int i = 0; i < nSubDiv; i++)
	{
		draw(false);
		t_start += step;
		t_end += step;
		sf::RectangleShape selection(sf::Vector2f(width_px, 100000));
		selection.setFillColor(sf::Color(255, 0, 0, 64));
		selection.setPosition(vTov2f(Vector3<>(t_start, 0, 0)));
		window.draw(selection);
		window.display();

		Vector3<> max_v(dft_max(t_start, t_end));
		dft_points.push_back(max_v);
		out_dft_pts.push_back(max_v);

		//lc::system::sleepSec(0.25);
	}
}

inline void Fitter::save_dft(std::string path, double wMin, double wMax, double dw)
{
	std::ofstream out_file(path, std::ifstream::out);
	for (double w = wMin; w < wMax; w += dw)
	{
		double A = fabs(dft_i(w));
		out_file << w << "\t" << A << std::endl;
	}
	out_file.close();
}

