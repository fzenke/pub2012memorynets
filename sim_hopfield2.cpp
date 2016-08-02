/* 
* Copyright 2012-2016 Friedemann Zenke
*
* This file is a simulation written for Auryn, a simulation package for plastic
* spiking neural networks.
* 
* Auryn is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Auryn is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with Auryn.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <string>

#include <boost/program_options.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi.hpp>

#include "auryn.h"

#define NE 8000
#define NI 2000

using namespace auryn;

namespace po = boost::program_options;
namespace mpi = boost::mpi;

int main(int ac,char *av[]) {
	std::string dir = ".";

	std::stringstream oss;
	std::string infilename = "";
	std::string strbuf ;
	std::string msg;

	double w = 0.1;
	double chi = 0.20;
	double gamma = 0.20;
	double ext = 10;

	double wee = 0.;
	double wei1 = w;
	double wei2 = 0.1*w;
	double wie = 1;
	double wii = 1.0;
	double w_ext = 0.1;

	double sparseness_e = 0.1;
	double sparseness_i = 0.1;
	double simtime = 500.;

	int errcode = 0;

	// BEGIN Global stuff
	mpi::environment env(ac, av);
	mpi::communicator world;
	communicator = &world;

	oss << dir  << "/spikehopf." << world.rank() << ".";
	std::string outputfile = oss.str();

	char tmp [255];
	std::stringstream logfile;
	logfile << outputfile << "log";
	logger = new Logger(logfile.str(),world.rank());

	sys = new System(&world);
	// END Global stuff
	

    try {

        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("load", po::value<std::string>(), "input weight matrix")
            ("chi", po::value<double>(), "chi factor")
            ("ext", po::value<double>(), "ext factor")
            ("gamma", po::value<double>(), "gamma factor")
            ("wee", po::value<double>(), "wee weight")
            ("wei1", po::value<double>(), "wei1 weight")
            ("wei2", po::value<double>(), "wei2 weight")
            ("wie", po::value<double>(), "wie weight")
            ("wii", po::value<double>(), "wii weight")
            ("simtime", po::value<double>(), "simulation time")
        ;

        po::variables_map vm;        
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);    

        if (vm.count("help")) {
			std::cout << desc << "\n";
            return 1;
        }

        if (vm.count("load")) {
			std::cout << "load from matrix " 
                 << vm["load"].as<std::string>() << ".\n";
			infilename = vm["load"].as<std::string>();
        } 

        if (vm.count("chi")) {
			std::cout << "chi from matrix " 
                 << vm["chi"].as<double>() << ".\n";
			chi = vm["chi"].as<double>();
        } 

        if (vm.count("ext")) {
			std::cout << "ext from matrix " 
                 << vm["ext"].as<double>() << ".\n";
			ext = vm["ext"].as<double>();
        } 

        if (vm.count("gamma")) {
			std::cout << "gamma factor " 
                 << vm["gamma"].as<double>() << ".\n";
			gamma = vm["gamma"].as<double>();
        } 

        if (vm.count("wee")) {
			std::cout << "wee factor " 
                 << vm["wee"].as<double>() << ".\n";
			wee = vm["wee"].as<double>();
        } 

        if (vm.count("wei1")) {
			std::cout << "wei1 factor " 
                 << vm["wei1"].as<double>() << ".\n";
			wei1 = vm["wei1"].as<double>();
        } 

        if (vm.count("wei2")) {
			std::cout << "wei2 factor " 
                 << vm["wei2"].as<double>() << ".\n";
			wei2 = vm["wei2"].as<double>();
        } 

        if (vm.count("wie")) {
			std::cout << "wie factor " 
                 << vm["wie"].as<double>() << ".\n";
			wie = vm["wie"].as<double>();
        } 

        if (vm.count("wii")) {
			std::cout << "wii factor " 
                 << vm["wii"].as<double>() << ".\n";
			wii = vm["wii"].as<double>();
        } 

        if (vm.count("simtime")) {
			std::cout << "simtime " 
                 << vm["simtime"].as<double>() << ".\n";
			simtime = vm["simtime"].as<double>();
        } 

    }
    catch(std::exception& e) {
		std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
		std::cerr << "Exception of unknown type!\n";
    }


	// double primetime = 10;
	logger->msg("Setting up neuron groups ...",PROGRESS,true);
	IFGroup * neurons_e = new IFGroup( NE );
	neurons_e->random_nmda(1,1);
	neurons_e->set_ampa_nmda_ratio(0.1);

	AIFGroup * neurons_i1 = new AIFGroup( NI/2 );
	neurons_i1->set_tau_mem(10e-3);
	neurons_i1->set_ampa_nmda_ratio(0.1);

	IFGroup * neurons_i2 = new IFGroup( NI/2 );
	neurons_i2->set_tau_mem(10e-3);
	neurons_i2->set_ampa_nmda_ratio(0.1);

	PoissonGroup * poisson = new PoissonGroup( 1000, ext);

	logger->msg("Setting up external connections ...",PROGRESS,true);
	SparseConnection * con_exte = new SparseConnection(poisson,neurons_e,w_ext,sparseness_e,GLUT);
	SparseConnection * con_exti2 = new SparseConnection(poisson,neurons_i2,w_ext,sparseness_i,GABA);

	logger->msg("Setting up EE connections ...",PROGRESS,true);
	SparseConnection * con_ee = new SparseConnection( neurons_e,neurons_e,wee,sparseness_e,GLUT); con_ee->set_name("EE");
	con_ee->load_patterns(infilename, chi);
	
	logger->msg("Setting up EI connections ...",PROGRESS,true);
	SparseConnection * con_ei1 = new SparseConnection( neurons_e,neurons_i1,wei1,sparseness_e,GLUT); con_ei1->set_name("EI1");
	SparseConnection * con_ei2 = new SparseConnection( neurons_e,neurons_i2,wei2,sparseness_e,GLUT); con_ei2->set_name("EI2");

	logger->msg("Setting up IE connections ...",PROGRESS,true);
	SparseConnection * con_i1e = new SparseConnection( neurons_i1,neurons_e,0.0,sparseness_i,GABA); con_i1e->set_name("I1E");
	SparseConnection * con_i2e = new SparseConnection( neurons_i2,neurons_e,wie,sparseness_i,GABA); con_i2e->set_name("I2E");
	con_i1e->load_patterns(infilename, gamma);

	logger->msg("Setting up II connections ...",PROGRESS,true);
	SparseConnection * con_i1i1 = new SparseConnection( neurons_i1,neurons_i1,wii,sparseness_i,GABA); con_i1i1->set_name("I1I1");
	SparseConnection * con_i2i2 = new SparseConnection( neurons_i2,neurons_i2,wii,sparseness_i,GABA); con_i2i2->set_name("I2I2");

	logger->msg("Setting up stimulus ...",PROGRESS,true);
	if ( infilename != "" ) {

		std::stringstream filename;
		filename << outputfile << "stimtimes";
		StimulusGroup * stimgroup = new StimulusGroup(1000,infilename,filename.str().c_str(),RANDOM);
		stimgroup->set_mean_on_period(1);
		stimgroup->set_mean_off_period(10);
		stimgroup->scale = 300;
		stimgroup->randomintervals=false;
		IdentityConnection * con_stim = new IdentityConnection(stimgroup,neurons_e,1.0,NMDA);

		std::vector<double> dist = stimgroup->get_distribution();
		int r = 3;
		for ( int i = 0 ; i < dist.size() ; ++i ) {
			if ( i < r ) 
				dist[i] = 0.99/r;
			else
				dist[i] = 0.01/(dist.size()-r);
		}
		stimgroup->set_distribution(dist);
		

		filename.str("");
		filename << outputfile << "stimtimes2";
		StimulusGroup * stimgroup2 = new StimulusGroup(1000,infilename,filename.str().c_str(),SEQUENTIAL);
		stimgroup2->set_mean_on_period(1);
		stimgroup2->set_mean_off_period(30);
		stimgroup2->scale = 200;
		SparseConnection * con_stim2 = new SparseConnection(stimgroup2,neurons_i2,0.1,sparseness_e);

		filename.str("");
		filename << outputfile << "pact";
		PatternMonitor * patmon = new PatternMonitor(neurons_e,filename.str().c_str(),infilename.c_str(),100);
	}



	msg = "Setting up monitors ...";
	logger->msg(msg,PROGRESS,true);

	std::stringstream filename;
	filename << outputfile << "e.ras";
	SpikeMonitor * smon_e = new SpikeMonitor( neurons_e, filename.str().c_str() );

	filename.str("");
	filename.clear();
	filename << outputfile << "i1.ras";
	SpikeMonitor * smon_i1 = new SpikeMonitor( neurons_i1, filename.str().c_str() );

	filename.str("");
	filename.clear();
	filename << outputfile << "i2.ras";
	SpikeMonitor * smon_i2 = new SpikeMonitor( neurons_i2, filename.str().c_str() );

	RateChecker * chk = new RateChecker( neurons_e , -0.1 , 1000. , 100e-3);

	for (int j = 0; j<1000 ; j++) {
	  neurons_e->tadd(j,5.);
	}

	msg = "Setting up monitors ...";
	logger->msg("Simulating ..." ,PROGRESS,true);
	if (!sys->run(simtime,true)) 
			errcode = 1;

	logger->msg("Freeing ..." ,PROGRESS,true);
	delete sys;

	if (errcode)
		env.abort(errcode);

	return errcode;
}
