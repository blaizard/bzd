# Sanitize images in bulk

This uitlity will sanitize the image library in bulk.

It will perform the following:

- Convert files to jpg format.
- Reduce the size of overly large files.
- Merge external exif metadata into the jpg file.
- Retain the creation/modification date.

## Use with exiftool

```bash
nix-shell -p exiftool
```
