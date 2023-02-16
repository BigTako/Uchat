#include "../inc/header.h"

code sha256_string(char *string)
{
    code hash = malloc(65*sizeof(char));
    unsigned char out[32];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, string, strlen(string));
    SHA256_Final(out, &sha256);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
	{
        sprintf(hash + (i * 2), "%02x", out[i]);
    }
    return hash;
}

EVP_PKEY * generate_key_pair()
{
    // create private/public key pair
    // init RSA context, so we can generate a key pair
    EVP_PKEY_CTX *keyCtx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY_keygen_init(keyCtx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(keyCtx, KEY_LENGHT);
    // variable that will hold both private and public keys
    EVP_PKEY *key = NULL;
    // generate key
    EVP_PKEY_keygen(keyCtx, &key);
    // free up key context
    EVP_PKEY_CTX_free(keyCtx);
    return key;
}

code PRIVKEY_to_str(EVP_PKEY * keypair)
{
    // extract private key as string
    // create a place to dump the IO, in this case in memory
    BIO *privateBIO = BIO_new(BIO_s_mem());
    // dump key to IO
    PEM_write_bio_PrivateKey(privateBIO, keypair, NULL, NULL, 0, 0, NULL);
    // get buffer length
    int privateKeyLen = BIO_pending(privateBIO);
    // create char reference of private key length
    code privateKeyChar = (code) malloc(privateKeyLen);
    // read the key from the buffer and put it in the char reference
    BIO_read(privateBIO, privateKeyChar, privateKeyLen);
    // at this point we can save the private key somewhere
    BIO_free(privateBIO);
    return privateKeyChar;
}

code PUBKEY_to_str(EVP_PKEY * keypair)
{
    // extract public key as string
    // create a place to dump the IO, in this case in memory
    BIO *publicBIO = BIO_new(BIO_s_mem());
    // dump key to IO
    PEM_write_bio_PUBKEY(publicBIO, keypair);
    // get buffer length
    int publicKeyLen = BIO_pending(publicBIO);
    // create char reference of public key length
    code publicKeyChar = (code) malloc(publicKeyLen);
    // read the key from the buffer and put it in the char reference
    BIO_read(publicBIO, publicKeyChar, publicKeyLen);
    // at this point we can save the public somewhere
    BIO_free(publicBIO);
    return publicKeyChar;
}

code encipher(code rsaPublicKeyChar, char * message, int * emLen, int * ekLen_v, code * ek_v, code * iv_v)
{
    // translate public key in string format to program structure to encrypt messsage
    // pretend we are pulling the public key from some source and using it
    // to encrypt a message
    // write char array to BIO
    BIO *rsaPublicBIO = BIO_new_mem_buf(rsaPublicKeyChar, -1);
    // create a RSA object from public key char array
    RSA *rsaPublicKey = NULL;
    PEM_read_bio_RSA_PUBKEY(rsaPublicBIO, &rsaPublicKey, NULL, NULL);
    // create public key
    EVP_PKEY *publicKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(publicKey, rsaPublicKey);
    // initialize encrypt context
    EVP_CIPHER_CTX* rsaEncryptCtx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(rsaEncryptCtx);
    // variables for where the encrypted secret, length, and IV reside
    code ek = (code) malloc(EVP_PKEY_size(publicKey));
    code iv = (code) malloc(EVP_MAX_IV_LENGTH);
    int ekLen = 0;
    // generate AES secret, and encrypt it with public key
    EVP_SealInit(rsaEncryptCtx, EVP_aes_256_cbc(), &ek, &ekLen, iv, &publicKey, 1);
    // encrypt a message with AES secret
    const unsigned char* messageChar = (const unsigned char*) message;
    // length of message
    int messageLen = strlen(message) + 1;
    // create char reference for where the encrypted message will reside
    code encryptedMessage = (code) malloc(messageLen + EVP_MAX_IV_LENGTH);
    // the length of the encrypted message
    int encryptedMessageLen = 0;
    int encryptedBlockLen = 0;
    // encrypt message with AES secret
    EVP_SealUpdate(rsaEncryptCtx, encryptedMessage, &encryptedBlockLen, messageChar, messageLen);
    encryptedMessageLen = encryptedBlockLen;
    // finalize by encrypting the padding
    EVP_SealFinal(rsaEncryptCtx, encryptedMessage + encryptedBlockLen, &encryptedBlockLen);
    encryptedMessageLen += encryptedBlockLen;
    *emLen = encryptedMessageLen;
    *ek_v = ek;
    *iv_v = iv;
    *ekLen_v = ekLen;
    BIO_free(rsaPublicBIO);
    EVP_PKEY_free(publicKey);
    EVP_CIPHER_CTX_free(rsaEncryptCtx);
    return encryptedMessage;
}

code decipher(code rsaPrivateKeyChar, code encryptedMessage, unsigned long emLen, code ek, code iv, int ekLen)
{
    // pretend we are decrypting a message we have received using a the private key we have
    // write char array to BIO
    BIO *rsaPrivateBIO = BIO_new_mem_buf(rsaPrivateKeyChar, -1);
    // create a RSA object from private key char array
    RSA *rsaPrivateKey = NULL;
    PEM_read_bio_RSAPrivateKey(rsaPrivateBIO, &rsaPrivateKey, NULL, NULL);
    // create private key
    EVP_PKEY *privateKey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(privateKey, rsaPrivateKey);
    // initialize decrypt context
    EVP_CIPHER_CTX* rsaDecryptCtx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(rsaDecryptCtx);
    // decrypt EK with private key, and get AES secretp
    EVP_OpenInit(rsaDecryptCtx, EVP_aes_256_cbc(), ek, ekLen, iv, privateKey);
    // variable for where the decrypted message with be outputed to
    code decryptedMessage = (code) malloc(emLen + EVP_MAX_IV_LENGTH);
    // the length of the encrypted message
    int decryptedMessageLen = 0;
    int decryptedBlockLen = 0;
    // decrypt message with AES secret
    EVP_OpenUpdate(rsaDecryptCtx, decryptedMessage, &decryptedBlockLen, encryptedMessage, emLen);
    decryptedMessageLen = decryptedBlockLen;
    // finalize by decrypting padding
    EVP_OpenFinal(rsaDecryptCtx, decryptedMessage + decryptedBlockLen, &decryptedBlockLen);
    decryptedMessageLen += decryptedBlockLen;
    BIO_free(rsaPrivateBIO);
    EVP_CIPHER_CTX_free(rsaDecryptCtx);
    EVP_PKEY_free(privateKey);
    return decryptedMessage;
}

/*
int main()
{
    EVP_PKEY * key = generate_key_pair();
    code privateKeyChar = PRIVKEY_to_str(key);
    code publicKeyChar = PUBKEY_to_str(key);
    code ek = NULL;
    code iv = NULL; 
    int emLen = 0; // encrypted message len
    int ekLen = 0; // encryption key len

    // scan a message wanted to encrypt
    char * message = malloc(MESSAGE_MAX_LEN*sizeof(char));
    
    printf("Enter message to encrypt: ");
    fgets(message, MESSAGE_MAX_LEN-1, stdin);
    message[strlen(message) - 1] = '\0';
    
    code encryptedMessage = encipher(publicKeyChar, message, &emLen, &ekLen, &ek, &iv);
    code decryptedMessage = decipher(privateKeyChar, encryptedMessage, emLen, ek, iv, ekLen);

    printf("\nPublic key: \n%s\n", publicKeyChar);
    printf("\n\nPrivate key: \n%s\n", privateKeyChar);
    printf("\nCipher: %s\n", encryptedMessage);
    printf("\nDecrypted message: %s\n", decryptedMessage);

    EVP_PKEY_free(key);
    free(message);
    free(encryptedMessage);
    free(decryptedMessage);
    free(privateKeyChar);
    free(publicKeyChar);
    free(ek);
    free(iv);
    return 0;
}*/

