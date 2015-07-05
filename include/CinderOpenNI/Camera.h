#pragma once

#include "cinder/gl/Texture.h"

namespace cinder {
    namespace openni {
        class Camera {
        public:
            Camera();
            ~Camera();

            static bool initialized;
            static void initialize();
            static void shutdown();

            void setup(int enableSensors=SENSOR_DEPTH|SENSOR_COLOR);
            void update();
            void close();

            ImageSourceRef getDepthImage();
            ImageSourceRef getRawDepthImage();
            ImageSourceRef getColorImage();
            gl::TextureRef & getDepthTex();
            gl::TextureRef & getRawDepthTex();
            gl::TextureRef & getColorTex();
            ivec2 getDepthSize(){ return getFrameData( depthIndex ).size; }
            ivec2 getColorSize(){ return getFrameData( colorIndex ).size; }

            enum SENSORS {
                SENSOR_DEPTH = 0x1,
                SENSOR_COLOR = 0x2
            };

			class CameraException : public std::exception {
			};
        private:

            _openni::Device device;
            _openni::VideoStream depthStream, colorStream;
            int depthIndex, colorIndex;

            class FrameDataAbstract {
            public:
                FrameDataAbstract( ivec2 size );

                ivec2 size;
                gl::TextureRef tex;
                ImageSourceRef imageRef;
                bool isImageFresh, isTexFresh;
                void initTexture( ivec2 size );
            };

            class FrameData : public FrameDataAbstract {
            public:
                FrameData( _openni::VideoStream &stream, ivec2 size );

                _openni::VideoStream &stream;
                _openni::VideoFrameRef frameRef;

                template < typename pixel_t, typename image_t >
                void updateImage();
                template < typename pixel_t, typename image_t >
                void updateTex();
            };

            class DerivedFrameData : public FrameDataAbstract {
            public:
                DerivedFrameData();
                ~DerivedFrameData();

                void updateOriginal( FrameData *_original );
                template < typename pixel_t, typename image_t, typename original_pixel_t >
                void updateImage();
                template < typename pixel_t, typename image_t, typename original_pixel_t >
                void updateTex();
            private:
                void convertData( const _openni::DepthPixel *originalData, uint8_t **convertedData );
                FrameData *original;
                uint8_t *convertedData;
            };

            std::vector< FrameData > all;
            _openni::VideoStream  **allStreams;
            DerivedFrameData scaledDepthFrameData;

            int setupStream( _openni::VideoStream &stream, _openni::SensorType sensorType );
            void updateStream( int streamIndex );
            FrameData & getFrameData( int index );
        };
    }
}