
# DES Encryption in C++

This project implements a simple `Des` class to manage encryption tasks using the Data Encryption Standard (DES). It includes two primary methods: `encryptDES` and `encryptFile`.

## Implementation Details

### encryptDES
The `encryptDES` method takes a plaintext string and a key as inputs, initializing the DES key schedule using OpenSSL's robust cryptographic functions. 

To conform to DES's 8-byte block size requirement, a padding mechanism similar to PKCS#7 is implemented, where the method appends padding bytes based on the number needed to complete the final block. This ensures that the plaintext aligns perfectly with DES's block boundaries, preventing data corruption during encryption.

### encryptFile
The `encryptFile` method reads the entire content of the input file into memory, applies the `encryptDES` method to generate the ciphertext, and then writes the encrypted data to an output file. This approach is effective for handling smaller files and is easily integrated into the broader inventory management system.

## Design Choices

- **Security and Performance**: OpenSSL's DES functions are used to ensure adherence to industry standards and leverage optimized performance.
- **Padding**: Implementing padding (PKCS#7-like) was crucial to handle varying plaintext lengths, ensuring data could be securely encrypted.
- **ECB Mode**: Electronic Codebook (ECB) mode was chosen for its simplicity, despite vulnerabilities like pattern leakage, to demonstrate block cipher encryption fundamentals.

## Recommendations

Several improvements are planned to enhance the class’s security and functionality:

1. **Transition to AES**: Moving from DES to Advanced Encryption Standard (AES) for stronger encryption and resistance against brute-force attacks.
2. **Improved Encryption Modes**: Switching from ECB to more secure modes like Cipher Block Chaining (CBC) or Galois/Counter Mode (GCM) to address ECB's known vulnerabilities.
3. **File Handling Optimization**: Enhancing the `encryptFile` method to handle large files block-wise for better memory efficiency and scalability.
4. **Error Handling**: Adding exceptions or detailed error codes for more resilient and debuggable encryption processes.
5. **Secure Key Management**: Implementing secure key storage (environment variables, secure vaults) to avoid hard-coded keys and protect sensitive data.

## Conclusion

This DES-based encryption class provides a foundation for understanding block cipher encryption and is integrated into an inventory management system. Future updates will align the implementation with modern security standards and practices, improving both security and scalability.
