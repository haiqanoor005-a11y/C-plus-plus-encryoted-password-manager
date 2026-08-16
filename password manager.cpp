#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <conio.h>
#include <limits> // for cin.ignore

using namespace std;

const string FILE_NAME = "passwords.txt";

// ==================================================
// HIDDEN PASSWORD INPUT
// ==================================================

string getHiddenPassword()
{
    string password;
    char ch;

    while (true)
    {
        ch = _getch();

        if (ch == 13) // ENTER
        {
            cout << endl;
            break;
        }

        if (ch == 8) // BACKSPACE
        {
            if (!password.empty())
            {
                password.erase(password.length() - 1, 1);
                cout << "\b \b";
            }
        }
        else
        {
            password += ch;
            cout << "*";
        }
    }

    return password;
}

// ==================================================
// XOR ENCRYPTION / DECRYPTION
// ==================================================

string xorCrypt(string text, string key)
{
    if (key.empty())
        return text;

    for (unsigned int i = 0; i < text.length(); i++)
    {
        text[i] = text[i] ^ key[i % key.length()];
    }

    return text;
}

// ==================================================
// TEXT TO HEX
// ==================================================

string toHex(string text)
{
    const char hex[] = "0123456789ABCDEF";

    string result;

    for (unsigned int i = 0; i < text.length(); i++)
    {
        unsigned char c = text[i];

        result += hex[(c >> 4) & 15];
        result += hex[c & 15];
    }

    return result;
}

// ==================================================
// HEX TO TEXT
// ==================================================

int hexValue(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;

    return -1;
}

string fromHex(string hex)
{
    string result;

    if (hex.length() % 2!= 0)
        return "";

    for (unsigned int i = 0; i < hex.length(); i += 2)
    {
        int high = hexValue(hex[i]);
        int low = hexValue(hex[i + 1]);

        if (high == -1 || low == -1)
            return "";

        char c = (char)((high << 4) | low);

        result += c;
    }

    return result;
}

// ==================================================
// SPLIT DATA
// ==================================================

vector<string> split(string text, char delimiter)
{
    vector<string> parts;
    string part;

    stringstream ss(text);

    while (getline(ss, part, delimiter))
    {
        parts.push_back(part);
    }

    return parts;
}

// ==================================================
// ADD PASSWORD
// ==================================================

void addPassword(string masterPassword)
{
    string website;
    string username;
    string password;

    cout << "\n======================================" << endl;
    cout << " ADD PASSWORD" << endl;
    cout << "======================================" << endl;

    cout << "Website: ";
    getline(cin, website);

    cout << "Username: ";
    getline(cin, username);

    cout << "Password: ";
    password = getHiddenPassword();

    if (website.empty() || username.empty() || password.empty())
    {
        cout << "\nAll fields are required." << endl;
        return;
    }

    // Combine information
    string data = website + "|" + username + "|" + password;

    // Encrypt
    string encrypted = xorCrypt(data, masterPassword);

    // Convert to HEX so it can safely be stored in the text file
    string encoded = toHex(encrypted);

    ofstream file(FILE_NAME.c_str(), ios::app);

    if (!file)
    {
        cout << "\nERROR: Could not open " << FILE_NAME << endl;
        return;
    }

    file << encoded << endl;

    file.close();

    cout << "\nPassword saved successfully!" << endl;
}

// ==================================================
// VIEW PASSWORDS
// ==================================================

void viewPasswords(string masterPassword)
{
    ifstream file(FILE_NAME.c_str());

    if (!file)
    {
        cout << "\nNo password file found." << endl;
        return;
    }

    string line;
    int count = 0;

    cout << "\n======================================" << endl;
    cout << " SAVED PASSWORDS" << endl;
    cout << "======================================" << endl;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // HEX -> encrypted text
        string encrypted = fromHex(line);

        if (encrypted.empty())
            continue;

        // Decrypt
        string decrypted = xorCrypt(encrypted, masterPassword);

        // Separate website, username and password
        vector<string> data = split(decrypted, '|');

        if (data.size()!= 3)
            continue;

        count++;

        cout << "\nEntry #" << count << endl;
        cout << "--------------------------------------" << endl;
        cout << "Website : " << data[0] << endl;
        cout << "Username: " << data[1] << endl;
        cout << "Password: " << data[2] << endl;
    }

    file.close();

    if (count == 0)
    {
        cout << "\nNo valid password entries found." << endl;
        cout << "Make sure you are using the same master password." << endl;
    }

    cout << "\n======================================" << endl;
}

// ==================================================
// SEARCH PASSWORD
// ==================================================

void searchPassword(string masterPassword)
{
    string searchTerm;

    cout << "\nEnter website to search: ";
    getline(cin, searchTerm);

    ifstream file(FILE_NAME.c_str());

    if (!file)
    {
        cout << "\nNo password file found." << endl;
        return;
    }

    string line;
    bool found = false;

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        string encrypted = fromHex(line);
        string decrypted = xorCrypt(encrypted, masterPassword);

        vector<string> data = split(decrypted, '|');

        if (data.size()!= 3)
            continue;

        if (data[0].find(searchTerm)!= string::npos)
        {
            cout << "\n======================================" << endl;
            cout << " PASSWORD FOUND" << endl;
            cout << "======================================" << endl;

            cout << "Website : " << data[0] << endl;
            cout << "Username: " << data[1] << endl;
            cout << "Password: " << data[2] << endl;

            found = true;
        }
    }

    file.close();

    if (!found)
    {
        cout << "\nNo matching website found." << endl;
    }
}

// ==================================================
// DELETE PASSWORD
// ==================================================

void deletePassword(string masterPassword)
{
    string searchTerm;

    cout << "\nEnter website to delete: ";
    getline(cin, searchTerm);

    ifstream input(FILE_NAME.c_str());

    if (!input)
    {
        cout << "\nNo password file found." << endl;
        return;
    }

    vector<string> remainingEntries;

    string line;
    bool found = false;

    while (getline(input, line))
    {
        if (line.empty())
            continue;

        string encrypted = fromHex(line);
        string decrypted = xorCrypt(encrypted, masterPassword);

        vector<string> data = split(decrypted, '|');

        if (data.size()!= 3)
        {
            remainingEntries.push_back(line);
            continue;
        }

        if (data[0].find(searchTerm)!= string::npos)
        {
            cout << "\nDeleting: " << data[0] << endl;

            found = true;
        }
        else
        {
            remainingEntries.push_back(line);
        }
    }

    input.close();

    if (!found)
    {
        cout << "\nNo matching website found." << endl;
        return;
    }

    ofstream output(FILE_NAME.c_str());

    if (!output)
    {
        cout << "\nERROR: Could not update password file." << endl;
        return;
    }

    for (unsigned int i = 0; i < remainingEntries.size(); i++)
    {
        output << remainingEntries[i] << endl;
    }

    output.close();

    cout << "Password deleted successfully!" << endl;
}

// ==================================================
// MAIN
// ==================================================

int main()
{
    string masterPassword;

    cout << "============================================" << endl;
    cout << " C++ PASSWORD MANAGER" << endl;
    cout << "============================================" << endl;

    cout << "\nEnter your master password: ";
    masterPassword = getHiddenPassword();

    if (masterPassword.empty())
    {
        cout << "\nMaster password cannot be empty." << endl;
        return 0;
    }

    int choice;

    while (true)
    {
        cout << "\n\n============================================" << endl;
        cout << " MAIN MENU" << endl;
        cout << "============================================" << endl;

        cout << "\n1. Add Password" << endl;
        cout << "2. View Passwords" << endl;
        cout << "3. Search Password" << endl;
        cout << "4. Delete Password" << endl;
        cout << "5. Exit" << endl;

        cout << "\nEnter your choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // FIX: Clear buffer

        if (choice == 1)
        {
            addPassword(masterPassword);
        }
        else if (choice == 2)
        {
            viewPasswords(masterPassword);
        }
        else if (choice == 3)
        {
            searchPassword(masterPassword);
        }
        else if (choice == 4)
        {
            deletePassword(masterPassword);
        }
        else if (choice == 5)
        {
            cout << "\nExiting Password Manager..." << endl;
            break;
        }
        else
        {
            cout << "\nInvalid choice. Please select 1-5." << endl;
        }
    }

    return 0;
}
