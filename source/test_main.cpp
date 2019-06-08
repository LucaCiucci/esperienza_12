/*
=======================================
|	General		C++						|
|	Author:		Luca Ciucci				|
|	Version:	0						|
|	Date:		x / x / 2019			|
|	e-mail:		develop@lucaciucci99.com|
|	licese:		license.md				|
=======================================
*/


#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <windows.h>
#include <iomanip>
#include <vector>
#include <string>

#include "LC/LC.h"
#include "fitter.h"
#include "fitter.inl"
#include "settings.h"

typedef double Np;

using namespace lc::math;
using namespace std::chrono;

int main(int argc, char** argv)
{
	for (int i = 0; i < files.size(); i += 1)
	{
		window_dft.clear();
		std::cout << path + files[i] << std::endl;
		Fitter fitter(path + files[i], speeds[i]);
		fitter.plot_dft(0, 10000);
#if false
		fitter.save_dft(std::string("dft_new/") + files[i], 0, 10);
#endif
		lc::math::FunctionOptimizator<> fo(chi2);
		fo.params[0] = 57.1999;
		fo.params[1] = 4.69884;
		fo.params[2] = 0.0665163;
		plot_f(fo.params[0], fo.params[1], fo.params[2]);
		window_dft.display();
		fitter.go();
	}
	window_dft.clear();
	lc::math::FunctionOptimizator<> fo(chi2);
	fo.params[0] = 70;
	fo.params[1] = 4.7;
	fo.params[2] = 0.1;
	//plot_f(fo.params[0], fo.params[1], fo.params[2]);
	//fitter.plot_dft(0, 10000);
	std::cout << "fiting con la funzione..." << std::endl;
	fo.optimize(0.05, 50000);
	plot_f(fo.params[0], fo.params[1], fo.params[2]);
	window_dft.display();
	lc::system::sleepSec(2);
	sf::Event event;
#if false
	for (int i = 0; i < files.size(); i += 1)
	{
		std::cout << files[i] << std::endl;
		Fitter fitter(files[i], speeds[i]);
		plot_f(fo.params[0], fo.params[1], fo.params[2]);
		while (window_dft.pollEvent(event));
		fitter.plot_dft(0, 10000);
		fitter.go();
	}
#endif

	std::ofstream out_file("dft_data.txt", std::ifstream::out);
	for (int i = 0; i < out_dft_pts.size(); i++)
	{
		out_file << out_dft_pts[i].x << "\t" << out_dft_pts[i].y << std::endl;
	}
	out_file.close();


	std::cout << "================================================================" << std::endl;
	std::cout << "                               END                              " << std::endl;
	std::cout << "================================================================" << std::endl;
	while (window_dft.isOpen())
	{
		while (window_dft.pollEvent(event))
			if (event.type == sf::Event::Closed)
				window_dft.close();
	}
	return 0;
	
}






















// OLD
int main_OLD(int argc, char** argv)
{
	//solo_ma_con_galleggiante.txt
	if (0)
	{
		{
			Fitter fitter("../../dati/solo_senza_compagnia.txt", 1);
			fitter.plot_dft(0, 10000);
			fitter.go();
		}
		{
			Fitter fitter("../../dati/solo_senza_compagnia.txt", 1);
			fitter.plot_dft(0, 10000);
			fitter.go();
		}
		double mean = 0.0;
		for (int i = 0; i < out_dft_pts.size(); i++)
		{
			mean += out_dft_pts[i].x;
		}
		mean /= out_dft_pts.size();
		double var = 0.0;
		for (int i = 0; i < out_dft_pts.size(); i++)
		{
			var += sqr(out_dft_pts[i].x - mean);
		}
		var /= sqr(out_dft_pts.size() - 1);

		std::cout << "w0: " << mean << " +- " << sqrt(var) << std::endl;
	}

	for (int i = 0; i < files.size(); i += 1)
	{
		std::cout << files[i] << std::endl;
		Fitter fitter(files[i], speeds[i]);
		fitter.plot_dft(0, 10000);
		fitter.go();
	}
	lc::math::FunctionOptimizator<> fo(chi2);
	fo.params[0] = 70;
	fo.params[1] = 4.7;
	fo.params[2] = 0.1;
	std::cout << "fiting..." << std::endl;
	fo.optimize(0.0005, 100000);
	sf::Event event;
	for (int i = 0; i < files.size(); i += 1)
	{
		std::cout << files[i] << std::endl;
		Fitter fitter(files[i], speeds[i]);
		plot_f(fo.params[0], fo.params[1], fo.params[2]);
		while (window_dft.pollEvent(event));
		fitter.plot_dft(0, 10000);
		fitter.go();
	}


	/*std::cout << files[14] << std::endl;
	Fitter fitter(files[14], speeds[14]);
	fitter.go();*/
	//for (double q = 3; q < 5; q += 0.001)
	{
		//std::cout << fabs(fitter.dft_i(q)) << std::endl;
	}
	//std::cout << fitter.dft_max(0, 100000.0) << std::endl;
	//fitter.plot_dft(0, 10000);
	//lc::system::sleepSec(2);
	//fitter.run_dft();

	std::ofstream out_file("dft_data.txt", std::ifstream::app);
	for (int i = 0; i < out_dft_pts.size(); i++)
	{
		out_file << out_dft_pts[i].x << "\t" << out_dft_pts[i].y << std::endl;
	}
	out_file.close();


	lc::math::FunctionOptimizator<> fo2(chi2);
	//fo2.oneStep(0.0005, 1000);

	plot_f(0.0, 0.0, 0.0);
	lc::system::sleepSec(1.0);
	while (1)
	{
		fo2.optimize(0.0005, 1000);
		plot_f(fo2.params[0], fo2.params[1], fo2.params[2]);
		while (window_dft.pollEvent(event));
	}

	return 0;
	VectorN<3> q;
	q.y;
}
