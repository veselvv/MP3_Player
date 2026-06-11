#include <mpg123.h>

double get_mp3_duration(const char* filename){
    mpg123_handle *mh = mpg123_new(NULL,NULL);
    if(!mh){
        return -1.0;
    }
    if(mpg123_open(mh,filename)!=MPG123_OK){
        mpg123_delete(mh);
        return -1.0;
    }
    long rate;
    int chanels, encoding;
    if(mpg123_getformat(mh, &rate, &chanels, &encoding)!=MPG123_OK){
        mpg123_close(mh);
        mpg123_delete(mh);
        return -1.0;
    }
    off_t total_samples = mpg123_length(mh);
    if(total_samples<0){
        mpg123_close(mh);
        mpg123_delete(mh);
        return -1.0;
    }
    double duration = (double)total_samples/rate;
    mpg123_close(mh);
    mpg123_delete(mh);
    return duration;
    
}