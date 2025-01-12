# Images

## Associate .xmp files with their .jpg

To associate .xmp file (metadata files for images) with their actual images, the following command can be run within the directory containing all images + their .xmp counterpart with the same name.

This works for a directory containing files named as follow:

```
- image1.jpg
- image1.jpg.xmp
- image_something.jpg
- image_something.jpg.xmp
```

```bash
nix-shell -p exiftool

# Move all dates and goetagging info from the .xmp file to the .jpg file.
exiftool -r -ext jpg -TagsFromFile %d%f.jpg.xmp "-AllDates<DateTimeOriginal" "-DateCreated<DateTimeOriginal" "-FileCreateDate<DateTimeOriginal" "-FileModifyDate<DateTimeOriginal" "-GPS:all<XMP-exif:all" "-GPS:GPSLatitudeRef<Composite:GPSLatitudeRef" "-GPS:GPSLongitudeRef<Composite:GPSLongitudeRef" "-GPS:GPSDateStamp<XMP-exif:GPSDateTime" "-GPS:GPSTimeStamp<XMP-exif:GPSDateTime" -overwrite_original .

# Visualise the result for a specific file.
exiftool file.jpg | grep -iE "(date|gps)"
```
