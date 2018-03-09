# Practice-BitmapImageProcessor

**This image processor is used for 24 bpp BMP file only**

- Practice C language
- Learn Bitmap storage structure

##### Usage: bmpedit [options...] [ifname]

    -o ofname      Sets the output file for modified images (default output)
    
    -t [0.0-1.0]   Apply a threshold filter to the image with a threshold
    
    -h             Displays this usage message
    
    -c [width start],[width end]:[height start],[height end]
                   Crops a specific part of a image，input fractions in proportion
                   e.g. 0.0,1.0:0.0,1.0 returns the original image
                   
    -l [-1.0-1.0]  Change the level of brightness of an image
    
    -b             Change a colured image to white-and-black

##### Last Update: 12/7/2016
