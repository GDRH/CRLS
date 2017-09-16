#include <SFML/Graphics.hpp>

#include <SFML/Audio.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

#include <time.h>
#include <stdlib.h>

#include <iostream>

std::vector<sf::Int16> m_sine;
std::vector<sf::Int16> m_silence;

void customMake( float freq ){
    m_sine.clear();
    for ( int i = 0; i < 11026; i++ ){
        //m_sine.push_back( sf::Int16( sin ( i*M_PI*freq/44100.f )* 16000.f ) );
        //m_sine.push_back( ( (i/(2200 - i/50 ) )%2 )*15000 );



        m_sine.push_back( ( ( i/ ( int(44100.f/freq) ) ) %2) * 8000.f - 4000.f );
        //m_sine.push_back( rand()%16000-8000 );
    }
}
std::size_t m_currentSample;

bool silenced = false;

// custom audio stream that plays a loaded buffer
class SynthStream : public sf::SoundStream
{
public:
    void load(){
        m_currentSample = 0;
        initialize( 1, 44100 );
    }

private:

    virtual bool onGetData(Chunk& data){
        const int samplesToStream = 2205;

        //std::cout << "Silenced " << silenced << "  currentSample : " << m_currentSample << std::endl;

        // set the pointer to the next audio samples to be played
        if ( !silenced )
            data.samples = &m_sine[m_currentSample];
        else{
            data.samples = &m_silence[m_currentSample];
        }

        // have we reached the end of the sound?
        if (m_currentSample + samplesToStream <= m_sine.size())
        {
            // end not reached: stream the samples and continue
            data.sampleCount = samplesToStream;
            m_currentSample += samplesToStream;
            return true;
        }
        else
        {
            // end of stream reached: stream the remaining samples and stop playback
            data.sampleCount = m_sine.size() - m_currentSample;

            silenced = true;
            m_currentSample = 0;
            //m_currentSample = m_sine.size();
            //return false;

            return true;
        }
    }

    virtual void onSeek(sf::Time timeOffset){
        m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    }
};

char keyOrder[] = "qwertyuiopasdfghjklzxcvbnm";

int getLn( int code ){
    for ( int i = 0; i < sizeof(keyOrder); i++ )
        if ( code+'a' == keyOrder[i] )
            return i;
    return 55;
}


#define RATIO 1.059463094359

int main()
{
    for ( int i = 0; i < 22050; i++ ){
        m_silence.push_back( 0 );
    }

    sf::RenderWindow window(sf::VideoMode(200, 200), "Custom Real Time Synth!");
    window.setFramerateLimit(60);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    customMake( 55.f );

    // initialize and play our custom stream
    SynthStream stream;
    stream.load();
    stream.play();

    srand(time(NULL));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                std::cout << "BOOP " <<  event.key.code << " " << getLn(event.key.code) << std::endl;
                customMake( 440 * pow ( RATIO, getLn( event.key.code ) ) );
                silenced = false;
                m_currentSample = 0;
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    //while (stream.getStatus() == MyStream::Playing)
        //sf::sleep(sf::seconds(0.1f));

    return 0;
}
