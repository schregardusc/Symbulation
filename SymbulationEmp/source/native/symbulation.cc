#include "../SymWorld.h"
#include "../../../Empirical/include/emp/config/ArgManager.hpp"
#include <iostream>

using namespace std;

// This is the main function for the NATIVE version of this project.

EMP_BUILD_CONFIG(SymConfigBase,
    VALUE(SEED, int, 10, "What value should the random seed be? If seed <= 0, then it is randomly re-chosen."),
    VALUE(DATA_INT, int, 100, "How frequently, in updates, should data print?"),
    VALUE(MUTATION_RATE, double, 0.002, "Standard deviation of the distribution to mutate by"),
    VALUE(VERT_MUTATION_RATE, double, 0.001, "Standard deviation of the distribution to mutate by"),
    VALUE(HORIZ_MUTATION_RATE, double, 0.005, "Standard deviation of the distribution to mutate by"),
    VALUE(SYNERGY, double, 5, "Amount symbiont's returned resources should be multiplied by"),
    VALUE(VERTICAL_TRANSMISSION, double, 1, "Value 0 to 1 of probability of symbiont vertically transmitting when host reproduces"),
    VALUE(HOST_INT, double, 0, "Interaction value from -1 to 1 that hosts should have initially, -2 for random"),
    VALUE(SYM_INT, double, 0, "Interaction value from -1 to 1 that symbionts should have initially, -2 for random"),
    VALUE(GRID_X, int, 5, "Width of the world, just multiplied by the height to get total size"),
    VALUE(GRID_Y, int, 5, "Height of world, just multiplied by width to get total size"),
    VALUE(UPDATES, int, 1, "Number of updates to run before quitting"),
    VALUE(SYM_LIMIT, int, 1, "Number of symbiont allowed to infect a single host"),
    VALUE(LYSIS, bool, 0, "Should lysis occur? 0 for no, 1 for yes"),
    VALUE(HORIZ_TRANS, bool, 1, "Should non-lytic horizontal transmission occur? 0 for no, 1 for yes"),
    VALUE(BURST_SIZE, int, 10, "If there is lysis, this is how many symbionts should be produced during lysis. This will be divided by burst_time and that many symbionts will be produced every update"),
    VALUE(BURST_TIME, int, 10, "If lysis enabled, this is how many updates will pass before lysis occurs"),
    VALUE(HOST_REPRO_RES, double, 1000, "How many resources required for host reproduction"),
    VALUE(SYM_LYSIS_RES, double, 1, "How many resources required for symbiont to create offspring for lysis each update"),
    VALUE(SYM_HORIZ_TRANS_RES, double, 100, "How many resources required for symbiont non-lytic horizontal transmission"),
    VALUE(START_MOI, int, 1, "Ratio of symbionts to hosts that experiment should start with"),
    VALUE(GRID, bool, 0, "Do offspring get placed immediately next to parents on grid, same for symbiont spreading"),
    VALUE(FILE_PATH, string, "", "Output file path"),
    VALUE(FILE_NAME, string, "_data_", "Root output file name")
)

int symbulation_main(int argc, char * argv[])
{    
  SymConfigBase config;
    
  bool success = config.Read("SymSettings.cfg");
  if(!success) config.Write("SymSettings.cfg");

  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "SymSettings.cfg") == false) {
    cerr << "There was a problem in processing the options file." << endl;
    exit(1);
  }
  if (args.TestUnknown() == false) {
    cerr << "Leftover args no good." << endl;
    exit(1);
  }
  if (config.BURST_SIZE()%config.BURST_TIME() != 0 && config.BURST_SIZE() < 999999999) {
  	cerr << "BURST_SIZE must be an integer multiple of BURST_TIME." << endl;
  	exit(1);
  }
// params
  int numupdates = config.UPDATES();
  int start_moi = config.START_MOI();
  double POP_SIZE = config.GRID_X() * config.GRID_Y();
  bool random_phen_host = false;
  bool random_phen_sym = false;
  if(config.HOST_INT() == -2) random_phen_host = true;
  if(config.SYM_INT() == -2) random_phen_sym = true;

  emp::Random random(config.SEED());
      
  SymWorld world(random);
  if (config.GRID() == 0) world.SetPopStruct_Mixed();
  else world.SetPopStruct_Grid(config.GRID_X(), config.GRID_Y());
// settings
  world.SetVertTrans(config.VERTICAL_TRANSMISSION());
  world.SetMutRate(config.MUTATION_RATE());
  world.SetVertMutRate(config.VERT_MUTATION_RATE());
  world.SetHorizMutRate(config.HORIZ_MUTATION_RATE());
  world.SetSymLimit(config.SYM_LIMIT());
  world.SetLysisBool(config.LYSIS());
  world.SetHTransBool(config.HORIZ_TRANS());
  world.SetBurstSize(config.BURST_SIZE());
  world.SetBurstTime(config.BURST_TIME());
  world.SetHostRepro(config.HOST_REPRO_RES());
  world.SetSymHRes(config.SYM_HORIZ_TRANS_RES());
  world.SetSymLysisRes(config.SYM_LYSIS_RES());
  world.SetSynergy(config.SYNERGY());

  world.SetResPerUpdate(100);

  int TIMING_REPEAT = config.DATA_INT();
  const bool STAGGER_STARTING_BURST_TIMERS = true;

  //Set up files
  //world.SetupPopulationFile().SetTimingRepeat(TIMING_REPEAT);

  if (config.LYSIS() == 1) {
    world.SetupLysisFile(config.FILE_PATH()+"Lysis"+config.FILE_NAME()+".data").SetTimingRepeat(TIMING_REPEAT);
  }
  world.SetupHostIntValFile(config.FILE_PATH()+"HostVals"+config.FILE_NAME()+".data").SetTimingRepeat(TIMING_REPEAT);
  world.SetupSymIntValFile(config.FILE_PATH()+"SymVals"+config.FILE_NAME()+".data").SetTimingRepeat(TIMING_REPEAT);

  

  //inject organisms
  for (size_t i = 0; i < POP_SIZE; i++){
    Host *new_org;
    if (random_phen_host) new_org = new Host(random.GetDouble(-1, 1));
    else new_org = new Host(config.HOST_INT());
    world.Inject(*new_org);
  }


  //This loop must be outside of the host generation loop since otherwise
  //syms try to inject into mostly empty spots at first
  int total_syms = POP_SIZE * start_moi;
  for (int j = 0; j < total_syms; j++){ 
      Symbiont new_sym; 
      if(random_phen_sym) new_sym = *(new Symbiont(random.GetDouble(-1, 1)));
      else new_sym = *(new Symbiont(config.SYM_INT()));
      if(STAGGER_STARTING_BURST_TIMERS)
        new_sym.burst_timer = random.GetInt(-5,5);
      world.InjectSymbiont(new_sym); 
    }

  //Loop through updates
  for (int i = 0; i < numupdates; i++) {
    if((i%TIMING_REPEAT)==0) {
      cout <<"Update: "<< i << endl;
      cout.flush();
    }
    world.Update();
  }

  return 0;
}

/*
This defenition guard prevents main from being defined twice during testing.
In testing, Catch will define a main function which will initiate tests
(including testing the symbulation_main function above).
*/
#ifndef CATCH_CONFIG_MAIN
int main(int argc, char * argv[]) {
  return symbulation_main(argc, argv);
}
#endif
