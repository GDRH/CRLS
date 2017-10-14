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

//Arduino Windows Serial Interface Library ( from ArduinoPlayground )
//https://playground.arduino.cc/Interfacing/CPPWindows
#include "SerialClass.h"

//Stores the waveform to be outputed on the audio stream
std::vector<sf::Int16> m_waveform;

//A 0-populated vector ( sound is off )
std::vector<sf::Int16> m_silence;

//This represents how many samples have been generated and must be streamed at each "onGetData"
//It is basically the wavelength multiplied by something for convenience
int generated = 2205;

//These control the streaming and note generation
int currentKey = sf::Keyboard::Q;
int lastKey = currentKey;
bool keyDown = false;

//This generates the samples, according to the frequency
void customMake( float freq ){
    m_waveform.clear(); // We clear the vector

    //Compute and store the wavelength multiplied by something...
    generated = 44100.f * 10.f / freq;

    //TODO : the wavelength above might cause issues on higher notes
    //FIX Solution : ( use a greater multiplier for those notes )
    //Find some exponential function or something like that

    //Then add the elements
    for ( int i = 0; i < generated*5+1; i++ ){ // Generated more than necessary to be sure

        //This is used to generate a sine wave of specific frequency
        m_waveform.push_back( sf::Int16( sin ( i*M_PI*freq/44100.f )* 16000.f ) );

        //This is used to generate a square wave of specific frequency
        //m_waveform.push_back( ( ( i/ ( int(44100.f/freq) ) ) %2) * 8000.f - 4000.f );

        //This is used to generate random noise ( no frequency )
        //m_waveform.push_back( rand()%16000-8000 );
    }
}

// custom audio stream that plays a loaded buffer
class SynthStream : public sf::SoundStream
{
public:
    void load(){
        //Initialize an audio output stream ( 1 = mono, 44100 = samples/second )
        initialize( 1, 44100 );
    }

private:

    virtual bool onGetData(Chunk& data){

        // set the pointer to the next audio samples to be played
        if ( keyDown ){
            data.samples = &m_waveform[0];
        }
        else{
            data.samples = &m_silence[0];
        }

        // set the sampleCount to the wavelength
        data.sampleCount = generated;
        return true;
    }

    //Internally used .... don't really have to care about it
    virtual void onSeek(sf::Time timeOffset){
        //m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
    }
};

//Stores the key order on the QWERTY keyboard ( so notes are in order )
char keyOrder[] = "qwertyuiopasdfghjklzxcvbnm";

/** Transforms the keycode to a note using the keyOrder */
int getLn( int code ){
    for ( unsigned int i = 0; i < sizeof(keyOrder); i++ )
        if ( code+'a' == keyOrder[i] )
            return i;
    return 0; // Default return value
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
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    customMake( 440.f ); // initialize the buffer for that frequency

    // initialize and play our custom stream
    SynthStream stream;
    stream.load();
    stream.play(); // This starts streaming, which cycles the m_silence if nothing is generated

    srand(time(NULL));

    Serial* SP = new Serial("\\\\.\\COM8");    // adjust as needed
	if (SP->IsConnected())
		std::cout << "Arduino connected" << std::endl;
	char incomingData[256] = "";
	int dataLength = 255;
	int readResult = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                currentKey = event.key.code;
                keyDown = true;
            }
            if (event.type == sf::Event::KeyReleased){
                if ( event.key.code == currentKey )
                    keyDown = false;
            }
        }

        if ( SP->IsConnected() ){
            readResult = SP->ReadData(incomingData,dataLength);
            // printf("Bytes read: (0 means no data available) %i\n",readResult);
            //incomingData[readResult] = 0;
            if ( readResult > 0 ){
                keyDown = 1 & ( incomingData[readResult-1] >> 7 );
                currentKey = 0b01111111 & incomingData[readResult-1];
                if ( lastKey != currentKey ){
                    customMake( 440 * pow ( RATIO, currentKey ) ); // compute the note frequency
                    lastKey = currentKey;
                }
            }
        }else if ( keyDown ){
                if ( lastKey != currentKey ){
                    std::cout << "Input " <<  currentKey << " " << getLn(currentKey) << std::endl;
                    customMake( 440 * pow ( RATIO, getLn( currentKey ) ) ); // compute the note frequency
                    lastKey = currentKey;
                }
        }

        //General update stuff
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}
