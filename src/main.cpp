
////////////////////////////////////////////////////////////
// sounda
////////////////////////////////////////////////////////////
#include "base_application.h"

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

#include "fft.h"

#define PI 3.141592653589793238462643

typedef std::vector<complex>    CVector;

// Main application
class MyApplication : public BaseApplication
{
public:
  MyApplication() = default;
  ~MyApplication() = default;

  int Run() override;

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

  void loadSound();
  void initHamming();
  void hammingWindow();
  void process(float max);
};

void MyApplication::loadSound()
{
  std::string fileName("resources/billy_talent_-_viking_death_march.ogg");
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

  for (int i(0); i < bufferSize; ++i)
    hammWindow.push_back(0.54 - 0.46 * cos(2 * PI * i / (float)bufferSize));
}

void MyApplication::hammingWindow()
{
  sf::Vector2f widgetPos = sf::Vector2f(20, 150);
  sf::Color clr(255, 255, 0, 50);

  float invWidgetWidth = 1. / (float)bufferSize * 760.;

  uint32_t mark = sound.getPlayingOffset().asSeconds() * sampleRate;
  if (mark + bufferSize < sampleCount) {
    for (int i(mark); i < bufferSize + mark; ++i) {
      sample[i - mark] = complex(buffer.getSamples()[i] * hammWindow[i - mark], 0);
      //VA1[i - mark] = sf::Vertex(widgetPos + sf::Vector2f((i - mark) * invWidgetWidth, sample[i - mark].re() * 0.005), clr);
      VA1[i - mark].position.x = widgetPos.x + (i - mark) * invWidgetWidth;
      VA1[i - mark].position.y = widgetPos.y + sample[i - mark].re() * 0.005;
      VA1[i - mark].color = clr;
    }
  }
}

void MyApplication::process(float max)
{
  VA2.clear();

  CFFT::Forward(sample.data(), sample.size());

	VA2.setPrimitiveType(sf::Lines);
	sf::Vector2f position(0, 450);
	for (float i(3); i < fmin(bufferSize / 2.f, 20000.f); i *= 1.01) {
		sf::Vector2f samplePosition(log(i) / log(fmin(bufferSize * .5f, 20000.f)), fabs(sample[(int)i].re()) / max);
		VA2.append(sf::Vertex(position + sf::Vector2f(samplePosition.x * 700, -samplePosition.y * 100.f), sf::Color::White));
		VA2.append(sf::Vertex(position + sf::Vector2f(samplePosition.x * 700, 0), sf::Color::White));
		VA2.append(sf::Vertex(position + sf::Vector2f(samplePosition.x * 700, 0), sf::Color(255,255,255,100)));
		VA2.append(sf::Vertex(position + sf::Vector2f(samplePosition.x * 700, samplePosition.y * 100.f * .5f), sf::Color(255,255,255,0)));
	}
}

int MyApplication::Run()
{
  sf::RenderWindow window(sf::VideoMode(800, 600, 32), "FFT demo");

  loadSound();
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
      break;

      // we don't process other types of events
      default:
        break;
      }
    }

    if (sound.getStatus() == sf::Sound::Playing) {
      hammingWindow();

      process(10000000.);

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
int main()
{
  std::cout << "Application started" << std::endl;
  int result = 0;
  try {
    MyApplication myApp;
    result = myApp.Run();
  }
  catch (...) {
    std::cerr << "Unhandled exception" << std::endl;
  }
  std::cout << "Done" << std::endl;
  return result;
}
