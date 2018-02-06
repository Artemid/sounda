
/**
 * sounda
 */

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "base_application.h"
#include "fft.h"

/*-------------------------------------------------------------------------*/
#define PI 3.141592653589793238462643

typedef std::vector<complex>    CVector;

/*-------------------------------------------------------------------------*/
// Main application
class MyApplication : public BaseApplication
{
public:
  MyApplication() = default;
  ~MyApplication() = default;

  int Run(int argc, char* argv[]) override;

private:
  sf::VertexArray VA1;
  sf::VertexArray VA2;

  std::vector<float> hammWindow;
  CVector sample;

  sf::SoundBuffer buffer;
  sf::Sound sound;
  uint64_t sampleRate;
  uint64_t sampleCount;
  uint64_t bufferSize;

  void loadSound(const std::string& fileName);
  void initHamming();
  void hammingWindow();
  void process(float max);
};

void MyApplication::loadSound(const std::string& fileName)
{
  //std::string fileName("resources/billy_talent_-_viking_death_march.ogg");
  if (!buffer.loadFromFile(fileName))
    return;

  sampleRate = buffer.getSampleRate() * buffer.getChannelCount();
  sampleCount = buffer.getSampleCount();
  bufferSize = 1024;//buffer.getSampleCount();

  // Display sound informations
  std::cout << fileName << ":" << std::endl;
  std::cout << " " << buffer.getDuration().asSeconds() << " seconds"       << std::endl;
  std::cout << " " << buffer.getSampleCount()          << " samples"       << std::endl;
  std::cout << " " << buffer.getSampleRate()           << " samples / sec" << std::endl;
  std::cout << " " << buffer.getChannelCount()         << " channels"      << std::endl;
}

void MyApplication::initHamming()
{
  VA1.setPrimitiveType(sf::LineStrip);
  VA1.resize(bufferSize);

  sample.resize(bufferSize);

  // Richar W. Hamming window function
  // see: https://en.wikipedia.org/wiki/Window_function#A_list_of_window_functions
  for (int i(0); i < bufferSize; ++i) {
    hammWindow.push_back(0.54 - 0.46 * cos(2. * PI * i / float(bufferSize - 1)));
  }
}

void MyApplication::hammingWindow()
{
  sf::Vector2f widgetPos = sf::Vector2f(20, 150);
  sf::Color clr(255, 255, 0, 127);

  float invWidgetWidth = 1. / (float)bufferSize * 760.;
  float invSampleAmp = 1. / 65535.;
  float sampleVal;

  uint32_t mark = sound.getPlayingOffset().asSeconds() * sampleRate;
  if (mark + bufferSize < sampleCount) {
    uint32_t l_ptr = mark;
    uint32_t r_ptr = mark + 1;
    for (int i(0); i < bufferSize; ++i) {
      sampleVal = float(buffer.getSamples()[l_ptr] + buffer.getSamples()[r_ptr]) * .5;
      l_ptr += 2;
      r_ptr += 2;
      sample[i] = complex(sampleVal * hammWindow[i], 0);
      clr.b = 127 + (int)((sampleVal + 32767.) * invSampleAmp * 255.);
      //VA1[i - mark] = sf::Vertex(widgetPos + sf::Vector2f((i - mark) * invWidgetWidth, sample[i - mark].re() * 0.005), clr);
      VA1[i].position.x = widgetPos.x + i * invWidgetWidth;
      VA1[i].position.y = widgetPos.y + sample[i].re() * 0.005;
      VA1[i].color = clr;
    }
  }
}

void MyApplication::process(float max)
{
  VA2.clear();
  VA2.setPrimitiveType(sf::Lines);

  CFFT::Forward(sample.data(), bufferSize);

  sf::Vector2f widgetPos(20, 450);

  // float xPos = fmin(bufferSize * .5f, 20000.f);
  // for (float i(3); i < xPos; i *= 1.01) {
  //   sf::Vector2f samplePosition(log(i) / log(xPos), log10(fabs(sample[(int)i >> 1].re())) * .3);
  //   VA2.append(sf::Vertex(widgetPos + sf::Vector2f(samplePosition.x * 700, -samplePosition.y * 100.f), sf::Color::White));
  //   VA2.append(sf::Vertex(widgetPos + sf::Vector2f(samplePosition.x * 700, 0), sf::Color::White));
  //   VA2.append(sf::Vertex(widgetPos + sf::Vector2f(samplePosition.x * 700, 0), sf::Color(255,255,255,100)));
  //   VA2.append(sf::Vertex(widgetPos + sf::Vector2f(samplePosition.x * 700, samplePosition.y * 100.f * .5f), sf::Color(255,255,255,0)));
  // }

  sf::Vertex vert;
  sf::Color clr(0, 0, 0, 127);
  float invWidgetWidth = 1. / (float)bufferSize;
  float val;
  for (int i(0); i < bufferSize; ++i) {
    val = sample[i >> 2].re() / max;
    sf::Vector2f samplePosition(i * invWidgetWidth * 760, fabs(val));

    vert.position.x = widgetPos.x + samplePosition.x;
    vert.position.y = widgetPos.y + -samplePosition.y;
    vert.color = sf::Color::White;
    VA2.append(vert);

    // vert.position.x = widgetPos.x + samplePosition.x;
    vert.position.y = widgetPos.y;
    vert.color = sf::Color::White;
    VA2.append(vert);

    // vert.position.x = widgetPos.x + samplePosition.x;
    // vert.position.y = widgetPos.y;
    // vert.color = sf::Color::White;
    VA2.append(vert);

    // vert.position.x = widgetPos.x + samplePosition.x;
    vert.position.y = widgetPos.y + samplePosition.y * .5f;
    vert.color = clr;
    VA2.append(vert);
  }
}

int MyApplication::Run(int argc, char* argv[])
{
  sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Hamming + FFT");

  loadSound(std::move(std::string(argv[1])));
  initHamming();

  sf::Event event;

  while (window.isOpen()) {
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;

      case sf::Event::KeyPressed:
      if (event.key.code == sf::Keyboard::P) {
        // Play a sound
        // Create a sound instance and play it
        sound.setBuffer(buffer);
        sound.play();
      }
      else if (event.key.code == sf::Keyboard::Escape) {
        window.close();
      }
      break;

      // we don't process other types of events
      default:
        break;
      }
    }

    if (sound.getStatus() == sf::Sound::Playing) {
      hammingWindow();

      process(10000.);

      // Display the playing position
      // std::cout << "\rPlaying... " << sound.getPlayingOffset().asSeconds() << " sec        ";
      // std::cout << std::flush;
    }

    window.clear();
    window.draw(VA1);
    window.draw(VA2);
    window.display();
  }

  // // Play music from an ogg file
  // playMusic("orchestral.ogg");

  // // Play music from a flac file
  // playMusic("ding.flac");

  // // Wait until the user presses 'enter' key
  // std::cout << "Press enter to exit..." << std::endl;
  // std::cin.ignore(10000, '\n');

  return EXIT_SUCCESS;
}


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
  std::cout << "Application started" << std::endl;
  int result = 0;
  try {
    MyApplication myApp;
    if (argc > 1) {
      result = myApp.Run(argc, argv);
    }
    else {
      std::cerr << "Application needs sound file name as argument" << std::endl;
    }
  }
  catch (...) {
    std::cerr << "Unhandled exception" << std::endl;
  }
  std::cout << "Done" << std::endl;
  return result;
}
