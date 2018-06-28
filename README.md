# stegobmp
## Installation
### Pre-requisites
* openssl 1.1.0
### Compilation
```
make
```
## Execution
### Embed
```
./stegobmp -embed -in secret.txt -p carrier.bmp -out output.bmp -steg LSB1 -a des -m cbc -pass password
```
### Extraction
```
./stegobmp -extract -p carrier.bmp -out message -steg LSB1 -a des -m cbc -pass password
```
### Parameters
 * **-in &lt;arg>**: Input file for embed mode.
 * **-p &lt;arg>**: BMP carrier.
 * **-out &lt;arg>**: Output file.
 * **-steg &lt;arg>**: Steganography algorithm (LSB1, LSB4, LSBE).
 * **-a &lt;arg>**: Encryption/Decryption algorithm (aes128, aes192, aes256, des).
 * **-m &lt;arg>**: Encryption/Decryption mode (ecb, cfb, ofb, cbc).
 * **-pass &lt;arg>**: Password.
 * **-h, --help: Display help.