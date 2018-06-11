cd src
gcc main.c parameters.c bmp_manager.c input_file_manager.c encryption.c steganography.c
cd ..
cp src/a.out ./
./a.out -p example.bmp -i msg.txt
