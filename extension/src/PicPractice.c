#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

//Basics of how to load an image
int main(void){
    int width, height, channels;
    unsigned char *img = stbi_load("stb_image/EveryItemImage.jpg", 
    &width, &height, &channels, 0);
    if (img == NULL){
        perror("failed to load image");
        exit(EXIT_FAILURE);
    }
    printf("loaded image with %d width, %d height and %d channels\n", width,height,channels);

    stbi_image_free(img);
}