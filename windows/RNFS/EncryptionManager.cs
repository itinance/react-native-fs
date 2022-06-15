using System;
using System.IO;
using System.Security.Cryptography;

public class EncryptionManager
{

    private const int BLOCKSIZE = 128;

    public int getIvLength()
    {
        return BLOCKSIZE / 8;
    }

    public byte[] generateIV()
    {
        using (Aes aes = Aes.Create())
        {
            aes.Mode = CipherMode.CBC;
            aes.Padding = PaddingMode.PKCS7;
            aes.GenerateIV();
            return aes.IV;
        }
    }

    public byte[] generateKey()
    {
        using (Aes aes = Aes.Create())
        {
            aes.Mode = CipherMode.CBC;
            aes.Padding = PaddingMode.PKCS7;
            aes.GenerateKey();
            return aes.Key;
        }
    }

    public byte[] readIv(FileStream fileStream)
    {
        return readIv(fileStream, getIvLength());
    }

    public byte[] readIv(FileStream fileStream, int ivLength)
    {
        var buffer = new byte[ivLength];
        fileStream.Read(buffer, 0, ivLength);
        return buffer;
    }

    public void writeIv(FileStream fileStream, byte[] iv)
    {
        fileStream.Write(iv, 0, iv.Length);
    }

    public CryptoStream getCryptoWriteStream(FileStream fileStream, String base64Key)
    {
        var key = Convert.FromBase64String(base64Key);
        return getCryptoWriteStream(fileStream, key);
    }

    public CryptoStream getCryptoWriteStream(FileStream fileStream, byte[] key)
    {
        var iv = generateIV();
        writeIv(fileStream, iv);
        return getCryptoWriteStream(fileStream, key, iv);
    }

    public CryptoStream getCryptoWriteStream(FileStream fileStream, byte[] key, byte[] iv)
    {
        CryptoStream csStream = null;
        using (Aes aes = Aes.Create())
        {
            aes.Key = key;
            aes.IV = iv;
            aes.Mode = CipherMode.CBC;
            aes.Padding = PaddingMode.PKCS7;
            ICryptoTransform decryptor = aes.CreateEncryptor(aes.Key, aes.IV);
            csStream = new CryptoStream(fileStream, decryptor, CryptoStreamMode.Write);
        }
        return csStream;
    }

    public CryptoStream getCryptoReadStream(FileStream fileStream, String base64Key)
    {
        var key = Convert.FromBase64String(base64Key);
        return getCryptoReadStream(fileStream, key);
    }

    public CryptoStream getCryptoReadStream(FileStream fileStream, byte[] key)
    {
        var iv = readIv(fileStream);
        return getCryptoReadStream(fileStream, key, iv);
    }

    public CryptoStream getCryptoReadStream(FileStream fileStream, byte[] key, byte[] iv)
    {
        CryptoStream csStream = null;
        using (Aes aes = Aes.Create())
        {
            aes.Key = key;
            aes.IV = iv;
            aes.Mode = CipherMode.CBC;
            aes.Padding = PaddingMode.PKCS7;
            ICryptoTransform decryptor = aes.CreateDecryptor(aes.Key, aes.IV);
            csStream = new CryptoStream(fileStream, decryptor, CryptoStreamMode.Read);
        }
        return csStream;
    }
}
