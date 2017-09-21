/** Custom Real-Time Synthesizer
    @date 21/09/2017

    @author Antonio Ganea
    @author Catalin Stoian

    @brief Generates and plays different waveforms depending on the keys pressed
*/

#include <SFML/Graphics.hpp>

//These are for the audio generating and streaming
#include <SFML/Audio.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

//These two are used for the srand() and rand() functions :
#include <time.h>
#include <stdlib.h>

//Debug console output :
#include <iostream>

//Stores the waveform to be outputed on the audio stream
std::vector<sf::Int16> m_waveform;

//A 0-populated vector ( sound is off )
std::vector<sf::Int16> m_silence;

void customMake( float freq ){
    m_waveform.clear(); // We clear the vector

    //Then add the elements
    for ( int i = 0; i < 11026; i++ ){

        //This is used to generate a sine wave of specific frequency
        //m_waveform.push_back( sf::Int16( sin ( i*M_PI*freq/44100.f )* 16000.f ) );

        //This is used to generate a square wave of specific frequency
        m_waveform.push_back( ( ( i/ ( int(44100.f/freq) ) ) %2) * 8000.f - 4000.f );

        //This is used to generate random noise ( no frequency )
        //m_waveform.push_back( rand()%16000-8000 );
    }
}
std::size_t m_currentSample;

//represents whether or not the synth is actually playing something
bool silenced = false;

// custom audio stream that plays a loaded buffer
class SynthStream : public sf::SoundStream
{
public:
    void load(){
        //start at the first sample in the m_waveform / m_silence vector
        m_currentSample = 0;

        //Initialize an audio output stream ( 1 = mono, 44100 = samples/second )
        initialize( 1, 44100 );
    }

private:

    virtual bool onGetData(Chunk& data){

        //At 44100 samples / second this should be equivalent to 50 ms of sound
        const int samplesToStream = 2205;

        // set the pointer to the next audio samples to be played
        if ( !silenced )
            data.samples = &m_waveform[m_currentSample];
        else{
            data.samples = &m_silence[m_currentSample];
        }

        // have we reached the end of the sound?
        if (m_currentSample + samplesToStream <= m_waveform.size())
        {
            // end not reached: stream the samples and continue
            data.sampleCount = samplesToStream;
            m_currentSample += samplesToStream;
            return true;
        }
        else
        {
            // end of stream reached: stream the remaining samples and stop playback
            data.sampleCount = m_waveform.size() - m_currentSample;

            silenced = true;
            m_currentSample = 0;

            return true;
        }
    }

    //Internally used .... don't really have to care about it
    virtual void onSeek(sf::Time timeOffset){
        m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    }
};

//Stores the key order on the QWERTY keyboard ( so notes are in order )
char keyOrder[] = "qwertyuiopasdfghjklzxcvbnm";


/** Transforms the keycode to a note using the keyOrder */
int getLn( int code ){
    for ( int i = 0; i < sizeof(keyOrder); i++ )
        if ( code+'a' == keyOrder[i] )
            return i;
    return 55; // Default return value ( don't ask why this )
}

/// Ratio used for generating note frequencies ( google this one up )
#define RATIO 1.059463094359

int main()
{
    // initialize empty m_silence vector ( samples for 0.5 seconds )
    for ( int i = 0; i < 22050; i++ ){
        m_silence.push_back( 0 );
    }

    //This is just to draw the window and test if it is working
    sf::RenderWindow window(sf::VideoMode(200, 200), "Custom Real Time Synth!");
    window.setFramerateLimit(60);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    customMake( 55.f ); // initialize the buffer for that frequency

    // initialize and play our custom stream
    SynthStream stream;
    stream.load();
    stream.play(); // This starts streaming, which cycles the m_silence if nothing is generated

    srand(time(NULL));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                //This prints the key code of the pressed button and the note number ( from 0 = A3 )
                std::cout << "Input " <<  event.key.code << " " << getLn(event.key.code) << std::endl;
                customMake( 440 * pow ( RATIO, getLn( event.key.code ) ) ); // compute the note frequency
                silenced = false; // make sure the SynthStream outputs sounds
                m_currentSample = 0; // seek the stream to the first element of m_waveform
            }
        }

        //General update stuff
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
