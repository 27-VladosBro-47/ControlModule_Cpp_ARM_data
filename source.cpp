//#include "mediapipe_hands_detector/mediapipe_hands_detector.h"
#include "hand_tracking/hand_tracking.h"
#include "altazimuth/altazimuth.h"

#include <filesystem>
#include <thread>

// Перевірка аргументів командного рядка
// Повертає string який позначає режим, у якому має працювати програма
std::string setMode(int argc, char** argv)
{
  if(argc > 1)
  {
    std::string agrument = argv[1];

    if(agrument == "default" || agrument == "DEFAULT" || agrument == "Default")
    {
      return "default";
    }
    else if(agrument == "learning" || agrument == "LEARNING" || agrument == "Learning")
    {
      return "learning";
    }
    else if(agrument == "make_data" || agrument == "MAKE_DATA" || agrument == "Make_data")
    {
      return "make_data";
    }
    else
    {
      std::cout << "Incorrect command line argument." << std::endl;
      std::cout << "Recognized as \"default\"." << std::endl;
      return "default";
    }
  }
  else
  {
    std::cout << "Command line argument is empty." << std::endl;
    std::cout << "Recognized as \"default\"." << std::endl;
    return "default";
  }

}


int main(int argc, char** argv)
{
  namespace fs = std::filesystem;

  std::string mode;
  mode = setMode(argc, argv); 

////========================================//
// namespace fs = std::filesystem;
// std::string buff =  fs::current_path();
// buff = buff + "/mediapipe/examples/desktop/controle_module/train_data_hands/0";

// fs::path pathToShow(buff);
// for (const auto & entry : std::filesystem::directory_iterator(pathToShow))
// {
//   std::cout << entry.path() << std::endl;
// }
////========================================//


////========================================//
// std::vector<double> data;
// data.resize(63);
// double extra = 0.1;
// for(auto &count : data)
// {
//   count = 1 + extra;
//   extra+=0.1;
// }

// saveTrainData("hand", 3, data); // 0
// saveTrainData("hand", 0, data); // 1
// saveTrainData("hand", 2, data); // 2
// saveTrainData("hand", 1, data); // 3
// saveTrainData("hand", 4, data); // 4
// saveTrainData("hand", 2, data); // 5
// saveTrainData("hand", 2, data); // 6
// saveTrainData("hand", 3, data); // 7
// saveTrainData("hand", 4, data); // 8
// saveTrainData("hand", 5, data); // 9
// saveTrainData("hand", 6, data); // 10
// saveTrainData("hand", 7, data); // 11
// saveTrainData("hand", 8, data); // 12
// saveTrainData("hand", 0, data); // 13
// saveTrainData("hand", 3, data); // 14
// saveTrainData("hand", 3, data); // 15
// saveTrainData("hand", 3, data); // 16
// saveTrainData("hand", 8, data); // 17
// saveTrainData("hand", 3, data); // 18
// saveTrainData("hand", 4, data); // 19
// saveTrainData("hand", 3, data); // 20

// data.clear();

// std::string type_out;
// int numb_out;

// loadTrainData("hand", 3, 4, type_out, numb_out, data);


// std::cout << "==============================\n";
// std::cout << "type_out = " << type_out << std::endl;
// std::cout << "numb_out = " << numb_out << std::endl;
// for(int i = 0; i < data.size(); i++)
// {
//   std::cout << "i[" << i << "] = " << data[i] << std::endl;
// }
// std::cout << "==============================\n";
// //========================================//

    std::vector<int> hiddenLayers;
    hiddenLayers.resize(3);
    hiddenLayers[0]=2;
    hiddenLayers[1]=2;
    hiddenLayers[2]=2;

    int inputPerceptrons = 63;
    int outputPerceptrons = 9;

  if(mode == "default")
  {
    std::cout << "Start the program in \"" << mode << "\" mode." << std::endl;

    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;
    std::cout << "gesture - 4" << std::endl;

    HandTracking handTracking(inputPerceptrons, outputPerceptrons, hiddenLayers);
    //AltAzimuth(handTracking.getIsWork(), handTracking.getPoint());
    //std::thread th1([&]()
    //{
      //AltAzimuth.mainLoop();
    //});
    //th1.detach();

    handTracking.processTracking();
    
  }
  else if(mode == "learning")
  {
    std::cout << "Start the program in \"" << mode << "\" mode." << std::endl;
    HandTracking handTracking(inputPerceptrons, outputPerceptrons, hiddenLayers);

    handTracking.processLearning();


  } 
  else if(mode == "make_data")
  {
    std::cout << "Start the program in \"" << mode << "\" mode." << std::endl;

  }





  return 0;
}