#include <string.h>
#include <openssl/evp.h>
#include <openssl/bio.h>

#define SUCCESS 1
#define FAILURE !SUCCESS
#define MAX_ENCR_LENGTH 1024

/* Stores the output in a file in base 64 */
int saveEncryptedData(unsigned char *out, int len, unsigned char *file_name)
{
    BIO *b64;
    BIO *bio;
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_file());
    if(bio == NULL)
        return FAILURE;
    if(!BIO_write_filename(bio, file_name))
        return FAILURE;
    bio = BIO_push(b64, bio);
    BIO_write(bio, out, len);
    BIO_flush(bio);
    BIO_free_all(bio);
    return SUCCESS;
}

/* Equivale a:
* openssl enc -aes-256-cbc -in in.txt -k margarita -e -a -md sha256 -nosalt 
*/
int main()
{
    unsigned char *output_file_name = "ciphered.txt";
    unsigned char *in = "Inteligencia, dame el nombre exacto de las cosas... Que mi palabra sea la cosa misma, creada por mi alma nuevamente.";
    unsigned char out[MAX_ENCR_LENGTH];
    int inl, outl, templ;
    unsigned char k[EVP_MAX_KEY_LENGTH];
    unsigned char iv[EVP_MAX_IV_LENGTH];
    const unsigned char *pwd = "margarita"; 
    const unsigned char *salt = NULL;
    const EVP_CIPHER *cipher = EVP_aes_256_cbc();
    const EVP_MD *dgst = EVP_sha256();
    int keylen, ivlen;    
    
    EVP_BytesToKey(cipher, dgst, salt, pwd, strlen(pwd),1, k, iv);
    EVP_CIPHER_CTX *ctx;
    /*inicializar contexto*/
    ctx = EVP_CIPHER_CTX_new();
    /*establecer parámetros de encripción en el contexto*/
    EVP_EncryptInit_ex(ctx, cipher, NULL, k, iv);
    inl = strlen(in);
    EVP_EncryptUpdate(ctx, out, &outl, in, inl);
    /*encripta 112 bytes*/
    printf("Encripta primero %d bytes\n", outl);
    /*encripta la parte final, lo que queda del bloque + padding*/
    EVP_EncryptFinal(ctx, out+outl, &templ);
    printf("Finalmente se encriptan %d bytes\n", templ);
    saveEncryptedData(out, outl+templ, output_file_name);
    /*limpiar estructura de contexto*/
    EVP_CIPHER_CTX_free(ctx);
    return EXIT_SUCCESS;
}