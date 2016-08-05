//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  CODE FILENAME: BatzelAssn6.cpp
//  DESCRIPTION:   This program will store real estate listing data into a linked list allowing a user to add, change, remove and modify listing data
//  CLASS/TERM:    CS362 / 14S8W1
//  DESIGNER:      Andrew C. Batzel
//  FUNCTIONS:     main- the main function makes function calls and loops the actionMenu function
//                 initialSetup- sets up the program for loading a file if the user chooses to do so
//                 openFile- loops while user enters the name of a file that exists to load data or until user chooses to not load data
//                 loadData- creates a linked list from the data that is in the text file chosen to load
//                 actionMenu- gives a list of options available to the user to choose from for adding, changing, removing, displaying and modifying listing data
//                 subMenu- contains a switch that will make call functions based on the actionMenu choice
//                 errorCheck- used to verify all user inputs are correct and formatted correctly
//                 displayListings- outputs to the display all the data within the linked lists
//                 addListing- adds another node to the linked list while placing it in ascending order
//                 removeListing- removes a node from the linked list
//                 changePrice- opens a text file that contains changes that will be made to the listing price of certain listings
//                 modifyListing- displays all listings and prompts user to choose one to modify
//                 modifyMenu- displays all options for modifications that can be made a listing
//                 modifySubMenu- loops through options user has to modifying data about a listing
//                 listReorder- will reorder the linked list used to store all listing data if the MLS number has been modified by the user
//                 exitProgram- writes a text file containing the data within the orderd linked list, de-allocates pointers and preps program to exit
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <cctype>
#include <cstddef>
#include <iomanip>
#include <stdlib.h>

using namespace std;

// constants
const int MAX_COMPANY_NAME = 20,     // max character length for a company name
          MAX_MLS_NUMBER = 6;        // max length of numbers for a MLS listing
const string LISTING_PRICE_CHANGES_FILE = "CHANGES.TXT";     // name of the file that contains changes to listing price
enum listingStatus {AVAILABLE, CONTRACT, SOLD};
enum errorCheckCategory {MLS_NUMBER_CHECK, LISTING_PRICE_CHECK, CURRENT_STATUS_CHECK, ZIP_CHECK, COMPANY_CHECK, ADD_ANOTHER};
struct listingData {
	int mlsNumber;
	float listingPrice;
	listingStatus status;
	string zip;
	string company;
	listingData* next;
};

// function prototypes
bool initialSetup();
bool openFile(ifstream& dataIn);
void loadData(ifstream& dataIn, listingData *&listTop);
char actionMenu(char& menuChoice, listingData *&listTop);
void subMenu(char menuChoice, listingData *&listTop);
void errorCheck(string& userInput, errorCheckCategory valueChecked, bool& goodInput, string& invalidReason, bool& firstPass);
void displayListings(listingData *listTop);
void addListing(listingData *&listTop);
void removeListing(listingData *&listTop);
void changePrice(listingData *&listTop);
void modifyListing(listingData *&listTop);
char modifyMenu();
void modifySubMenu(listingData *&listTop, listingData *currentPtr, char menuChoice);
void listReorder(listingData *&listTop, listingData *currentPtr);
void exitProgram(listingData *&listTop);

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     main
//  DESCRIPTION:  the main function makes function calls and loops the actionMenu function
//  OUTPUT:
//       Return Val:   Returns the value 0 upon sucessful completion of the program.
//  CALLS TO:     initialSetup
//                openFile
//                loadData
//                actionMenu
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
	char menuChoice;                   // choice selected from actionMenu
    bool openExistingFile = false,     // bool for is user wants to load existing data
		fileOpened = false;            // bool for if the file was loaded
	ifstream dataIn;                   // read data from a file
	listingData *listTop = NULL;       // pointer to the first node
    
    // ask user if they want to load existing data
	openExistingFile = initialSetup();
	// if load existing data is true
	if (openExistingFile)
		fileOpened = openFile(dataIn);
	// load existing data into an ordered link list
	if (fileOpened)
	   loadData(dataIn, listTop);
    // loop through menu while user doesn't want to exit the program
    do
    {
       actionMenu(menuChoice, listTop);
    }while (menuChoice != 'E');
	
	
	system("pause");
	return 0;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     initialSetup
//  DESCRIPTION:  sets up the program for loading a file if the user chooses to do so
//  OUTPUT:
//       Return Val:   true if user wants to load a file, false if user doesn't want to load a file
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

bool initialSetup()
{
	bool openExistingFile;     // bool for load existing data
	char loadFile;             // stores user input
	
	do
	{
		cout << "Load existing data from file (Y/N)? ";
		cin >> loadFile;
		loadFile = toupper(loadFile);
		if ( (loadFile != 'Y') && (loadFile != 'N') )
			cout << "ERROR - Invalid character selection" << endl;
	}while ( (loadFile != 'Y') && (loadFile != 'N') );
	
	loadFile == 'Y' ? openExistingFile = true: openExistingFile = false;     // set to true if user wants to load existing data, false if not
	
	return openExistingFile;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     openFile
//  DESCRIPTION:  loops while user enters the name of a file that exists to load data or until user chooses to not load data
//  INPUT:
//       Parameter:    dataIn- variable used to check if file entered by a user already exists
//  OUTPUT:
//       Return Val:   true if the file entered by the user exists, false if the file doesn't exist
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

bool openFile(ifstream& dataIn)
{
	string fileName;     // store the name of the file to be read from
	char menuOption;     
	bool fileExists,
		escapeToMenu;
	
	do
	{
		fileExists = false;
		escapeToMenu = false;
        cout << "Enter file name to load: ";
		cin >> fileName;
		dataIn.open(fileName.c_str());
		if (dataIn)
			fileExists = true;
		else
		{
			do
			{
				cout << fileName << " does not exist." << endl
                     << "(R) Reenter new file name, or (E) escape to main menu? ";
				cin >> menuOption;
				menuOption = toupper(menuOption);
				if ( (menuOption != 'R') && (menuOption != 'E') )
					cout << "ERROR - Invalid character selection" << endl;
			}while ( (menuOption != 'R') && (menuOption != 'E') );
			if (menuOption == 'E')
				escapeToMenu = true;
			else
			{
                dataIn.clear();
				escapeToMenu = false;
            } // end reenter file name
		} // end file doesn't exist
	}while( (!fileExists) && (!escapeToMenu) );
	
	
	return fileExists;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     loadData
//  DESCRIPTION:  creates a linked list from the data that is in the text file chosen to load
//  INPUT:
//       Parameter:    dataIn- variable used to read data from a text file chosen by a user
//                     listTop- pointer used to locate the top of the linked list
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void loadData(ifstream& dataIn, listingData *&listTop)
{
	char pauseChar;          // used when memory allocation fails
	int tempMlsNumber;
	float tempListedPrice;
	int tempStatus;
	listingStatus status1;
	string tempZip,
           tempCompany;
    listingData *newNode,
                *currentPtr,
                *prevPtr;
	
	if (!dataIn)
	   cout << "ERROR - Data file did not open." << endl;
    else
    {
        do
        {
            // read from file into temp variables
            dataIn >> tempMlsNumber;
            dataIn >> tempListedPrice;
            dataIn >> tempStatus;
            status1 = static_cast<listingStatus>(tempStatus);
            dataIn >> tempZip;
            dataIn.ignore();
            getline(dataIn, tempCompany);
            
            if(dataIn)
            {
                newNode = new (nothrow) listingData;
                if (newNode == NULL)
                {
                    cout << "HEAP ERROR - Could not allocate memory." << endl;
                    cin >> pauseChar;
                } // end if new node is empty
                else
                {
                    newNode->mlsNumber = tempMlsNumber;
                    newNode->listingPrice = tempListedPrice;
                    newNode->status = status1;
                    newNode->zip = tempZip;
                    newNode->company = tempCompany;
                    newNode->next = NULL;
                    if (listTop == NULL)
                    {
                        currentPtr = newNode;
                        listTop = newNode;
                    } // end if the first node is empty
                    else
                    {
                         if (newNode->mlsNumber < listTop->mlsNumber)
                         {
                             newNode->next = listTop;
                             listTop = newNode;
                         } // end if new node's MLS listing is less than the first node
                         else
                         {
                            prevPtr = listTop;
                            currentPtr = listTop->next;
                            while ( (currentPtr != NULL) && (newNode->mlsNumber > currentPtr->mlsNumber) )
                            {
                                  prevPtr = currentPtr;
                                  currentPtr = currentPtr->next;
                            } // finds where to insert the newly added node
                            newNode->next = prevPtr->next;
                            prevPtr->next = newNode;
                         } // end if new node's MLS listing is not less than the first node
                    } // end if the first node is not empty
                } // end if the new node's memory allocation was successful
            } // end if there is data still to read in the file
        }while ( dataIn && (newNode != NULL) );
        dataIn.close();
    } // end if file opened

    return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     actionMenu
//  DESCRIPTION:  gives a list of options available to the user to choose from for adding, changing, removing, displaying and modifying listing data
//  INPUT:
//       Parameter:    menuChoice- passed via reference so it can loop in the main function until user wants to exit
//                     listTop- pointer used to locate the top of the linked list
//  OUTPUT:
//       Return Val:   menuChoice- the character value of the user selected choice from the menu options
//  CALLS TO:     subMenu
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

char actionMenu(char& menuChoice, listingData *&listTop)
{	
	bool isEmpty,
		 validInput = false;
	
	if (listTop == NULL)
		isEmpty = true;
	else
		isEmpty = false;
	
	system("cls");
    cout << "-------------------------------ACTION MENU-------------------------------" << endl << endl;
	if (!isEmpty)
		cout << "D - Display all listings" << endl;
	cout << "A - Add listing" << endl;
	if (!isEmpty)
	{
		cout << "R - Remove listing" << endl;
		cout << "C - Change asking price" << endl;
		cout << "M - Modify listing" << endl;
	}
	cout << "E - Exit program" << endl << endl;
    do
    {
         cout << "Choose an option from above: ";
         cin >> menuChoice;
         menuChoice = toupper(menuChoice);
         if ( (menuChoice != 'D') && (menuChoice != 'A') && (menuChoice != 'R') && (menuChoice != 'C') && (menuChoice != 'E') && (menuChoice != 'M'))
            cout << "ERROR - Invalid character selection." << endl;
         if(isEmpty && ( (menuChoice == 'D') || (menuChoice == 'R') || (menuChoice == 'C') || (menuChoice == 'M') ) )
         {
         	validInput = false;
         	cout << "ERROR - Invalid selection." << endl;
         } // end if invalid menu choice was selected while list is empty
         else
         	validInput = true;
         	
    }while (!validInput);
    
    // call subMenu with menu choice that was selected
    subMenu(menuChoice, listTop);
    
    return menuChoice;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     subMenu
//  DESCRIPTION:  contains a switch that will make call functions based on the actionMenu choice
//  INPUT:
//       Parameter:    menuChoice- the character value of the user selected choice from the menu options
//                     listTop- pointer used to locate the top of the linked list
//  CALLS TO:     displayListings
//                addListing
//                removeListing
//                changePrice
//                modifyListing
//                exitProgram
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void subMenu(char menuChoice, listingData *&listTop)
{
	switch (menuChoice)
	{
           case 'D':
                displayListings(listTop);
                break;
           case 'A':
                addListing(listTop);
                break;
           case 'R':
                removeListing(listTop);
                break;
           case 'C':
                changePrice(listTop);
                break;
           case 'M':
                modifyListing(listTop);
                break;
           default:
                   exitProgram(listTop);
    }
    return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     errorCheck
//  DESCRIPTION:  used to verify all user inputs are correct and formatted correctly
//  INPUT:       
//      Parameter:  userInput
//                  valueChecked
//                  goodInput
//                  invalidReason
//                  firstPass
//  OUTPUT:      This function will pass via reference goodInput, invalidReason and firstPass
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void errorCheck(string& userInput, errorCheckCategory valueChecked, bool& goodInput, string& invalidReason, bool& firstPass)
{
     float listingPrice;
     
     switch (valueChecked)
     {
            case MLS_NUMBER_CHECK:
                 if (userInput.length() > MAX_MLS_NUMBER)
                    invalidReason = "MLS# has too many digits. Must be 6 digits long.";
                 else if (userInput.length() < MAX_MLS_NUMBER)
                      invalidReason = "MLS# has too few digits. Must be 6 digits long.";
                 else if (userInput[0] == '0')
                      invalidReason = "MLS# may not begin with a 0.";
                 else if ( !isdigit(userInput[0]) ||  !isdigit(userInput[1]) || !isdigit(userInput[2]) || !isdigit(userInput[3]) || !isdigit(userInput[4]) || !isdigit(userInput[5]) )
                      invalidReason = "MLS# must contain all numberical digits.";
                 else 
                      goodInput = true;
                 break;
            case LISTING_PRICE_CHECK:
                 for (int i = 0; i < userInput.length(); i++)
                 {
                      if ( (userInput[i] >= '0') && (userInput[i] <= '9') )
                           goodInput = true;
                      else
                      {
                          goodInput = false;
                          invalidReason = "MLS# must contain all numberical digits.";
                          return;
                      }
                 }
                 break;
            case CURRENT_STATUS_CHECK:
                 if (userInput.length() > 1)
                    invalidReason = "Too many characters selected. Try Again.";
                 else if ( (userInput[0] != 'A') && (userInput[0] != 'C') && (userInput[0] != 'S') )
                      invalidReason = "Invalid character selected. Try Again.";
                 else
                 {
                     goodInput = true;
                     switch(userInput[0])
                     {
                          case 'A':
                               userInput = '0';
                               break;
                          case 'C':
                               userInput = '1';
                               break;
                          case 'S':
                               userInput = '2';
                               break;
                     }
                 }
                 break;
            case ZIP_CHECK:
                 if (userInput.length() > 10)
                    invalidReason = "Zip code may only contain 10 characters. Try again.";
                 else if (userInput.length() < 10)
                      invalidReason = "Zip code must contain 10 characters. Try again.";
                 else if ( !isdigit(userInput[0]) ||  !isdigit(userInput[1]) || !isdigit(userInput[2]) || !isdigit(userInput[3]) || !isdigit(userInput[4]) || (userInput[5] != '-') ||  !isdigit(userInput[6]) || !isdigit(userInput[7]) || !isdigit(userInput[8]) || !isdigit(userInput[9]) )
                      invalidReason = "Zip code format incorrect. Must be #####-####. Try again.";
                 else
                     goodInput = true;
                 break;
            case COMPANY_CHECK:
                 if (userInput.length() > MAX_COMPANY_NAME)
                 {
                      cout << "Company name may only be up to 20 characters long. Try again.";
                      goodInput = false;
                      firstPass = false;
                 }
                 else
                 {
                     for (int i = 0; i < (userInput.length() - 1); i++)
                     {
                         if (!isalpha(userInput[i]) && !isspace(userInput[i]))
                         {
                              goodInput = false;
                              firstPass = false;
                              invalidReason = "Company name many only contain letters and spaces. Try again.";
                              return;
                         }
                         else
                         {
                             if (i == 0)
                                userInput[i] = toupper(userInput[i]);
                             else if (isspace(userInput[i]))
                             {
                                  i++;
                                  if (isalpha(userInput[i]))
                                     userInput[i] = toupper(userInput[i]);
                                  else
                                      i--;
                             }
                             else
                                 userInput[i] = tolower(userInput[i]); 
                         }
                     }
                     goodInput = true;
                 }
                 break;
            case ADD_ANOTHER:
                    if (userInput.length() > 1)
                       invalidReason = "Too many characters selected. Try again.";
                    else if ( (userInput[0] != 'Y') && (userInput[0] != 'N') )
                         invalidReason = "Invalid character selected. Try again.";
                    else
                        goodInput = true;
                        break;
     }
     
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     displayListings
//  DESCRIPTION:  outputs to the display all the data within the linked lists
//  INPUT:
//       Parameter:    listTop- pointer used to locate the top of the linked list
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void displayListings(listingData *listTop)
{
	listingData *currentPtr;
    string listingStatus;
	
    currentPtr = listTop;
    
    system("cls");
    cout << left << setw(6) << "" << setw(3) << "" << setw(7) << "Asking" << setw(3) << "" << setw(9) << "Listing" << setw(3) << "" << setw(10) << "" << setw(3) << "" << setw(20) << "" << endl
         << left << setw(6) << "MLS#" << setw(3) << "" << setw(7) << "Price" << setw(3) << "" << setw(9) << "Status" << setw(3) << "" << setw(10) << "Zip Code" << setw(3) << "" << setw(20) << "Realtor" << endl
         << left << setw(6) << "------" << setw(3) << "" << setw(7) << "-------" << setw(3) << "" << setw(9) << "---------" << setw(3) << "" << setw(10) << "----------" << setw(3) << "" << setw(20) << "--------------------" << endl;
    if (listTop == NULL)
        cout << "No listings stored." << endl;
     else
     {
         	while (currentPtr != NULL)
	        {     
                 // sets the string listingStatus based on the enum value in current array member
                 switch (currentPtr->status)
                 {
                      case AVAILABLE:
                           listingStatus = "Available";
                           break;
                      case CONTRACT:
                           listingStatus = "Contract";
                           break;
                      default:
                           listingStatus = "Sold";
                 }
                 cout << left << setw(6) << currentPtr->mlsNumber << setw(3) << "" << right << setw(7) << currentPtr->listingPrice << setw(3) << "" << left << setw(9) << listingStatus << setw(3) << "" << setw(10) << currentPtr->zip << setw(3) << "" << setw(20) << currentPtr->company << endl;
                 currentPtr = currentPtr->next;
            } // end for loop to display all listings data
     } // end if first node is not empty

    system("pause");
	return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     addListing
//  DESCRIPTION:  adds another node to the linked list while placing it in ascending order
//  INPUT:
//       Parameter:    listTop- pointer used to locate the top of the linked list
//  CALLS TO:     errorCheck
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void addListing(listingData *&listTop)
{
	 listingData *currentPtr,
                 *prevPtr,
                 *newNode,
                 *tempPtr;
	 int tempMlsNumber,
         tempMlsCompare;
     float tempPrice;
     string addMlsNumber,
            addCurrentStatus,
            addListingPrice,
            addZip,
            addCompany,
            invalidReason,
            addAnother = "Y";
     bool goodInput,
          firstPass,
          existing,
          targetFound = false;
     char pauseChar;
     
     if (listTop == NULL)
     {
         currentPtr = listTop;
     }
     else
     {
         currentPtr = listTop;
         while (currentPtr->next != NULL)
               currentPtr = currentPtr->next;
     }
           
     system("cls");
     cout << "----------------------------------ADD LISTING----------------------------------" << endl;
     
     do
     {
           newNode = new (nothrow) listingData;
           if (newNode == NULL)
           {
               cout << "HEAP ERROR - Could not allocate memory." << endl;
               cin >> pauseChar;
           } // end if new node is empty
           else
           {
               firstPass = true;
               do
               {
                   existing = false;
                   goodInput = false;
                   do
                   {
                     cout << "Enter MLS#:  ";
                     cin >> addMlsNumber;
                     errorCheck(addMlsNumber, MLS_NUMBER_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
                   } while (!goodInput);
                   tempMlsNumber = atoi(addMlsNumber.c_str());
                   tempPtr = listTop;
                   while ( (tempPtr != NULL) && !existing)
                   {
                       if (tempPtr->mlsNumber == tempMlsNumber)
                          existing = true;
                       else
                           tempPtr = tempPtr->next;
                   }
                       
                   if (existing)
                      cout << "MLS# is already listed. Can only add a MLS# not already listed." << endl;
               } while (existing);
               newNode->mlsNumber = tempMlsNumber;
               
               goodInput = false;
               do
               {
                     cout << "Enter listing price:  ";
                     cin >> addListingPrice;
                     errorCheck(addListingPrice, LISTING_PRICE_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
                     tempPrice = atof(addListingPrice.c_str());
                     if (tempPrice <= 0)
                     {
                         cout << "Listing price cannot be free. Must be greater than 0." << endl;
                         goodInput = false;
                     }
               } while (!goodInput);
               newNode->listingPrice = atof(addListingPrice.c_str());
               
               goodInput = false;
               do
               {
                     cout << "Enter listing status (A) Available, (C) Contract or (S) Sold:  ";
                     cin >> addCurrentStatus;
                     addCurrentStatus[0] = toupper(addCurrentStatus[0]);
                     errorCheck(addCurrentStatus, CURRENT_STATUS_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
               } while (!goodInput);
               newNode->status = static_cast<listingStatus>(atoi(addCurrentStatus.c_str()));
               
               goodInput = false;
               do
               {
                     cout << "Enter listing zip code (#####-####):  ";
                     cin >> addZip;
                     errorCheck(addZip, ZIP_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
               } while (!goodInput);
               newNode->zip = addZip;
               
               goodInput = false;
               do
               {
                     cout << "Enter realty company name (Up to 20 characters):  ";
                     if (firstPass)
                        cin.ignore();
                     getline (cin, addCompany);
                     errorCheck(addCompany, COMPANY_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
               } while (!goodInput);
               newNode->company = addCompany;
               
               newNode->next = NULL;
               
               goodInput = false;
              do
              {
                  cout << "Would you like to add another listing (Y/N)?  ";
                  cin >> addAnother;
                  addAnother[0] = toupper(addAnother[0]);
                  errorCheck(addAnother, ADD_ANOTHER, goodInput, invalidReason, firstPass);
                  if (!goodInput)
                     cout << invalidReason << endl;
              } while (!goodInput);
      
              if (listTop == NULL)
              {
                   currentPtr = newNode;
                   listTop = newNode;
               }
               else
               {
                    if (newNode->mlsNumber < listTop->mlsNumber)
                    {
                        newNode->next = listTop;
                        listTop = newNode;
                    }
                    else
                    {
                       prevPtr = listTop;
                       currentPtr = listTop->next;
                       while ( (currentPtr != NULL) && (newNode->mlsNumber > currentPtr->mlsNumber) )
                       {
                             prevPtr = currentPtr;
                             currentPtr = currentPtr->next;
                       }
                       newNode->next = prevPtr->next;
                       prevPtr->next = newNode;
                    }
               }
            } // end if memory allocation of new node was good
         }while (addAnother[0] == 'Y');

     return;   
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     removeListing
//  DESCRIPTION:  removes a node from the linked list
//  INPUT:
//       Parameter:    listTop- pointer used to locate the top of the linked list
//  CALLS TO:     errorCheck
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void removeListing(listingData *&listTop)
{
	 listingData *prevNode,
                 *tempNode,
                 *currentNode;
     int mlsCompare,
         mlsRemovedInt,
         lineCount = 0;
     string mlsRemovedStr,
            invalidReason;
     bool firstPass,
          goodInput = false,
          mlsFound = false,
          targetFound = false;
     
     system("cls");
     cout << "---------------------------------REMOVE LISTING---------------------------------" << endl;
     if (listTop == NULL)
        cout << "No listings are stored on record." << endl;
     else
     {
         currentNode = listTop;
         while (currentNode != NULL)
         {
             if (lineCount % 8 == 0)
                cout << endl;
             cout << left << setw(10) << currentNode->mlsNumber;
             currentNode = currentNode->next;
             lineCount++;
         }
     }
     cout << endl << endl;
     prevNode = listTop;
     currentNode = listTop;
     do
     {
          cout << "Enter MLS# of listing to be removed:  ";
          cin >> mlsRemovedStr;
          errorCheck(mlsRemovedStr, MLS_NUMBER_CHECK, goodInput, invalidReason, firstPass);
                 if (!goodInput)
                    cout << invalidReason << endl;
     } while (!goodInput);
     mlsRemovedInt = atoi(mlsRemovedStr.c_str());
     // find target
     while ( (currentNode != NULL) && !targetFound)
     {
         if (currentNode->mlsNumber == mlsRemovedInt)
            targetFound = true;
         else
         {
             targetFound = false;
             prevNode = currentNode;
             currentNode = currentNode->next;
         }
     }

     if (targetFound)
     {
         if (currentNode == listTop)
         {
             listTop = listTop->next;
             delete currentNode;
         }
         else
         {
             tempNode = currentNode;
             currentNode = currentNode->next;
             prevNode->next = currentNode;
             delete tempNode;
         }
             
          cout << "MLS# " << mlsRemovedInt << " removed from records." << endl;
     }
     cout << "MLS# " << mlsRemovedInt << " not found in records." << endl;
     system("pause");
          
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     changePrice
//  DESCRIPTION:  opens a text file that contains changes that will be made to the listing price of certain listings
//  INPUT:
//       Parameter:    listTop- pointer used to locate the top of the linked list
//       File:         dataIn- a file containing data about the reduction in price for certain listings
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void changePrice(listingData *&listTop)
{
	ifstream dataIn;
	listingData *currentPtr;
	int tempMlsNumber;
	float priceChange;
	bool targetFound,
         changeMade = false,
         displayChange = false;
	
	dataIn.open(LISTING_PRICE_CHANGES_FILE.c_str());
	
	if(!dataIn)
        cout << LISTING_PRICE_CHANGES_FILE << " file does not exist." << endl;
    else
    {
        dataIn >> tempMlsNumber >> priceChange;
        cout << left << setw(15) << "MLS number" << right << setw(15) << "New Asking Price" << endl
             << left << setw(15) << "----------" << right << setw(15) << "----------------" << endl;
        do
        {
            targetFound = false;
            currentPtr = listTop; 
            while ( (currentPtr != NULL) && !targetFound)
            {
                if (currentPtr->mlsNumber == tempMlsNumber)
                {
                   if ( (currentPtr->listingPrice - priceChange) <= 0 )
                   {
                        targetFound = true;
                        displayChange = false;
                   }
                   else
                   {
                       currentPtr->listingPrice = currentPtr->listingPrice - priceChange;
                       targetFound = true;
                       changeMade = true;
                       displayChange = true;
                   }
                }
                else
                    currentPtr = currentPtr->next;
            }
            if (targetFound)
            {
               cout << left << setw(15) << tempMlsNumber << right << setw(15) << currentPtr->listingPrice;
               if (!displayChange)
                  cout << "*price change cannot bring listing price below 1." << endl;
               else
                   cout << endl;
            }
            dataIn >> tempMlsNumber >> priceChange;
            
        }while (dataIn);
    }
    dataIn.close();
    if (!changeMade)
       cout << "No price reductions were made." << endl;
    
    system("pause");

        
    return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     modifyListing
//  DESCRIPTION:  displays all listings and prompts user to choose one to modify
//  INPUT:
//       Parameter:    listTop- pointer used to locate the top of the linked list
//  CALLS TO:     errorCheck
//                modifyMenu
//                modifySubMenu
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void modifyListing(listingData *&listTop)
{
     listingData *currentPtr,
                 *currentNode;
     int lineCount = 0,
         mlsModifyInt;
     string mlsModifyStr,
            invalidReason,
            listingStatus;
     bool goodInput,
          firstPass,
          targetFound;
     char menuChoice;
     
     system("cls");
     cout << "---------------------------------MODIFY LISTING---------------------------------" << endl;
     if (listTop == NULL)
        cout << "No listings are stored on record." << endl;
     else
     {
         currentNode = listTop;
         while (currentNode != NULL)
         {
             if (lineCount % 8 == 0)
                cout << endl;
             cout << left << setw(10) << currentNode->mlsNumber;
             currentNode = currentNode->next;
             lineCount++;
         }
     }
     cout << endl << endl;
     do
     {
          goodInput = false;
          targetFound = false;
          cout << "Enter MLS# of listing to be modified:  ";
          cin >> mlsModifyStr;
          errorCheck(mlsModifyStr, MLS_NUMBER_CHECK, goodInput, invalidReason, firstPass);
          if (!goodInput)
             cout << invalidReason << endl;
     } while (!goodInput);
     mlsModifyInt = atoi(mlsModifyStr.c_str());
     currentPtr = listTop;
     while ( (currentPtr != NULL) && !targetFound)
     {
         if (currentPtr->mlsNumber == mlsModifyInt)
            targetFound = true;
         else
             currentPtr = currentPtr->next;
     }
     if (targetFound)
     {
         do
         {
             switch (currentPtr->status)
             {
                  case AVAILABLE:
                       listingStatus = "Available";
                       break;
                  case CONTRACT:
                       listingStatus = "Contract";
                       break;
                  default:
                       listingStatus = "Sold";
             }
             system("cls");
             cout << left << setw(6) << "" << setw(3) << "" << setw(7) << "Asking" << setw(3) << "" << setw(9) << "Listing" << setw(3) << "" << setw(10) << "" << setw(3) << "" << setw(20) << "" << endl
                  << left << setw(6) << "MLS#" << setw(3) << "" << setw(7) << "Price" << setw(3) << "" << setw(9) << "Status" << setw(3) << "" << setw(10) << "Zip Code" << setw(3) << "" << setw(20) << "Realtor" << endl
                  << left << setw(6) << "------" << setw(3) << "" << setw(7) << "-------" << setw(3) << "" << setw(9) << "---------" << setw(3) << "" << setw(10) << "----------" << setw(3) << "" << setw(20) << "--------------------" << endl
                  << left << setw(6) << currentPtr->mlsNumber << setw(3) << "" << right << setw(7) << currentPtr->listingPrice << setw(3) << "" << left << setw(9) << listingStatus << setw(3) << "" << setw(10) << currentPtr->zip << setw(3) << "" << setw(20) << currentPtr->company << endl << endl;
             
                  menuChoice = modifyMenu();
                  modifySubMenu(listTop, currentPtr, menuChoice);
         }while (menuChoice != 'R');
     
     
     }
     else
     {
         cout << "MLS# not found." << endl;
         system("pause");
     }

     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     modifyMenu
//  DESCRIPTION:  displays all options for modifications that can be made a listing
//  OUTPUT:
//       Return Val:   menuChoice- the character value of the option selected by the user
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

char modifyMenu()
{
     char menuChoice;
     
     cout << "----------------------------------MODIFY MENU-----------------------------------" << endl << endl
          << "M - MLS Number" << endl
	      << "L - Listing Price" << endl
	      << "S - Listing Status" << endl
	      << "Z - Zip Code" << endl
	      << "C - Company" << endl
	      << "R - Return to Main Menu" << endl << endl;
     do
     {
         cout << "Choose a modify option from above: ";
         cin >> menuChoice;
         menuChoice = toupper(menuChoice);
         if ( (menuChoice != 'M') && (menuChoice != 'L') && (menuChoice != 'S') && (menuChoice != 'Z') && (menuChoice != 'C') && (menuChoice != 'R'))
            cout << "ERROR - Invalid character selection." << endl;
     }while ( (menuChoice != 'M') && (menuChoice != 'L') && (menuChoice != 'S') && (menuChoice != 'Z') && (menuChoice != 'C') && (menuChoice != 'R'));
    
     
     return menuChoice;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     modifySubMenu
//  DESCRIPTION:  loops through options user has to modifying data about a listing
//  INPUT:
//       Parameters:   listTop- pointer used to locate the top of the linked list
//                     currentPtr- pointer used to locate the node that is being modified
//                     menuChoice- the character value of the option selected by the user
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void modifySubMenu(listingData *&listTop, listingData *currentPtr, char menuChoice)
{
     listingData *tempPtr;
     bool existing = false,
          goodInput = false,
          mlsChange = false,
          firstPass = true;
     string addMlsNumber,
            addListingPrice,
            addCurrentStatus,
            addZip,
            addCompany,
            invalidReason;
     int tempMlsNumber;
     float tempPrice;
     
     switch (menuChoice)
     {
            case 'M':
                 do
                 {
                     existing = false;
                     goodInput = false;
                     do
                     {
                         cout << "Enter new MLS#:  ";
                         cin >> addMlsNumber;
                         errorCheck(addMlsNumber, MLS_NUMBER_CHECK, goodInput, invalidReason, firstPass);
                         if (!goodInput)
                            cout << invalidReason << endl;
                     } while (!goodInput);
                     tempMlsNumber = atoi(addMlsNumber.c_str());
                     tempPtr = listTop;
                     while ( (tempPtr != NULL) && !existing)
                     {
                         if (tempPtr->mlsNumber == tempMlsNumber)
                            existing = true;
                         else
                             tempPtr = tempPtr->next;
                     }                          
                     if (existing)
                        cout << "MLS# already exists." << endl;
                 } while (existing);
                 currentPtr->mlsNumber = tempMlsNumber;
                 mlsChange = true;
                 break;
            case 'L':
                 do
                 {
                     cout << "Enter listing price:  ";
                     cin >> addListingPrice;
                     errorCheck(addListingPrice, LISTING_PRICE_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
                     tempPrice = atof(addListingPrice.c_str());
                     if (tempPrice <= 0)
                     {
                         cout << "Listing price cannot be free. Must be greater than 0." << endl;
                         goodInput = false;
                     }
                 } while (!goodInput);
                 currentPtr->listingPrice = tempPrice;
                 break;
            case 'S':
                 do
                 {
                     cout << "Enter listing status (A) Available, (C) Contract or (S) Sold:  ";
                     cin >> addCurrentStatus;
                     addCurrentStatus[0] = toupper(addCurrentStatus[0]);
                     errorCheck(addCurrentStatus, CURRENT_STATUS_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
                 } while (!goodInput);
                 currentPtr->status = static_cast<listingStatus>(atoi(addCurrentStatus.c_str()));
                 break;
            case 'Z':
                 do
                 {
                     cout << "Enter listing zip code (#####-####):  ";
                     cin >> addZip;
                     errorCheck(addZip, ZIP_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
                 } while (!goodInput);
                 currentPtr->zip = addZip;
                 break;
            case 'C':
                 do
                 {
                     cout << "Enter realty company name (Up to 20 characters):  ";
                     if (firstPass)
                        cin.ignore();
                     getline (cin, addCompany);
                     errorCheck(addCompany, COMPANY_CHECK, goodInput, invalidReason, firstPass);
                     if (!goodInput)
                        cout << invalidReason << endl;
                 } while (!goodInput);
                 currentPtr->company = addCompany;
                 break;
     }

     if (mlsChange)
        listReorder(listTop, currentPtr);
     
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     listReorder
//  DESCRIPTION:  will reorder the linked list used to store all listing data if the MLS number has been modified by the user
//  INPUT:
//       Parameters:   listTop- pointer used to locate the top of the linked list
//                     currentPtr- pointer used to locate the node that is being modified
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void listReorder(listingData *&listTop, listingData *currentPtr)
{
     listingData *pointer1,
                 *pointer2;
   
     if (currentPtr->mlsNumber < listTop->mlsNumber)
     {
         pointer1 = listTop;
         while (pointer1->next != currentPtr)
               pointer1 = pointer1->next;
         pointer1->next = currentPtr->next;
         currentPtr->next = listTop;
         listTop = currentPtr;
     }
     else
     {
         pointer1 = listTop;
         while (pointer1->next != currentPtr)
               pointer1 = pointer1->next;
         pointer1->next = currentPtr->next;
         
         pointer1 = listTop;
         pointer2 = listTop->next;
         while (pointer2->mlsNumber <= currentPtr->mlsNumber)
         {
               pointer1 = pointer2;
               pointer2 = pointer2->next;
         }
         currentPtr->next = pointer2;
         pointer1->next = currentPtr;
     }

     
     return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------
//  FUNCTION:     exitProgram
//  DESCRIPTION:  writes a text file containing the data within the orderd linked list, de-allocates pointers and preps program to exit
//  INPUT:
//       Parameters:   listTop- pointer used to locate the top of the linked list
//  OUTPUT:
//       File:         dataOut- variable used to output data within the linked list to a text file
//--------------------------------------------------------------------------------------------------------------------------------------------------------------

void exitProgram(listingData *&listTop)
{
	listingData *currentPtr;
	char saveOption,
         overwrite;
    bool goodFile = false;
    ofstream textOut;
    ifstream textIn;
    string saveFile;
     
     do
     {
         cout << "Would you like to save data to file(Y/N)?  ";
         cin >> saveOption;
         saveOption = toupper(saveOption);
         if ( (saveOption != 'Y') && (saveOption != 'N') )
            cout << "Invalid character selection. Try again." << endl;
     }while ( (saveOption != 'Y') && (saveOption != 'N') );
     
     if(saveOption == 'N')
          cout << "All changes made to listings will be disregarded." << endl;
     else
     {
          do
          {
              cout << "What would you like the saved file to be called? ";
              cin >> saveFile;
              textIn.open(saveFile.c_str());
              if (textIn)
              {
                 do
                 {
                     cout << saveFile << " already exists. Overwrite data in existing file (Y/N)? ";
                     cin >> overwrite;
                     overwrite = toupper(overwrite);
                     if ( (overwrite != 'Y') && (overwrite != 'N') )
                         cout << "Invalid character selection. Try again." << endl;
                 }while ( (overwrite != 'Y') && (overwrite != 'N') );
                 if (overwrite == 'Y')
                    goodFile = true;
              }
              else
                  goodFile = true;
              textIn.close();
          }while (!goodFile);
          
          textOut.open(saveFile.c_str());
          currentPtr = listTop;
          while (currentPtr != NULL)
          {
                textOut << currentPtr->mlsNumber << " " << currentPtr->listingPrice << " " << currentPtr->status << " " << currentPtr->zip << " " << currentPtr->company << endl;
                currentPtr = currentPtr->next;
          }
          textOut.close();             
     }
     
     currentPtr = listTop;
     while (currentPtr != NULL)
     {
     	listTop = currentPtr->next;
     	delete currentPtr;
     	currentPtr = listTop;
     }
     delete currentPtr;
	 listTop = NULL;
     
     return;
}
