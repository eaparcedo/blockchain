#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>
#include <random>
#include <iomanip>
#include <numeric>

// ==============================================================================================================
// 
// Demonstration of building, finding and printing blockchains.
// 
// --------------------------------------------------------------------------------------------------------------
// Compiled and tested under Ubuntu v18.04, g++ v7.3, clang version 6.0.0-1ubuntu2 and Windows 10 Professional 
// with Visual Studio 2017 v15.9.4.
// --------------------------------------------------------------------------------------------------------------
//
// USAGE:
// -----
//
// ./blockchain [Number of Blocks] [Max number of transactions per block]
//
// RUNTIME RESULTS:
// ---------------
//
// PROMPT> ./blockchain
//
// Blockchain      [CurrentID      | PreviousID    | Transactions]
//
// Block 1:       0xD449ED84       | 0xFFFFFFFF    | B 47 EOS, B 31 TRX
// Block 2:       0xCD098E70       | 0xD449ED84    | B 82 TRX, S 75 BCH, B 26 ETH
// Block 3:       0xDB2D2585       | 0xCD098E70    | B 28 LTC, B 65 XVG, B 62 NEM
// Block 4:       0x78D4E931       | 0xDB2D2585    | B 56 BSV, B 56 TRX, B 39 DASH, S 22 ETH
// Block 5:       0x80243B6B       | 0x78D4E931    | S 65 EOS
// Block 6:       0x9F9FA7AB       | 0x80243B6B    | B 85 NEO, B 11 TRX, S 69 TRX
// Block 7:       0x9879F78F       | 0x9F9FA7AB    | S 92 BSV
// Block 8:       0xBC7278A1       | 0x9879F78F    | S 63 ETH, B 62 LTC, B 13 BTC, B 9 BSV
// Block 9:       0xE43F992D       | 0xBC7278A1    | S 26 DASH
// Block A:       0x80CAE46D       | 0xE43F992D    | B 43 ADA, S 31 NEM, B 83 ETC, S 70 XRP, B 38 XMR
//
// Block vector [CurrentID | PreviousID    | Transactions]
//
// Block:       0x80CAE46D | 0xE43F992D    | B 43 ADA, S 31 NEM, B 83 ETC, S 70 XRP, B 38 XMR
// Block:       0xE43F992D | 0xBC7278A1    | S 26 DASH
// Block:       0xBC7278A1 | 0x9879F78F    | S 63 ETH, B 62 LTC, B 13 BTC, B 9 BSV
// Block:       0x9879F78F | 0x9F9FA7AB    | S 92 BSV
// Block:       0x9F9FA7AB | 0x80243B6B    | B 85 NEO, B 11 TRX, S 69 TRX
// Block:       0x80243B6B | 0x78D4E931    | S 65 EOS
// Block:       0x78D4E931 | 0xDB2D2585    | B 56 BSV, B 56 TRX, B 39 DASH, S 22 ETH
// Block:       0xDB2D2585 | 0xCD098E70    | B 28 LTC, B 65 XVG, B 62 NEM
// Block:       0xCD098E70 | 0xD449ED84    | B 82 TRX, S 75 BCH, B 26 ETH
// Block:       0xD449ED84 | 0xFFFFFFFF    | B 47 EOS, B 31 TRX
//
//
// Ernesto L. Aparcedo, Ph.D. (c) 2019 - All Rights Reserved				   
// 
// ==============================================================================================================

using namespace std;

// Class that contains a block of multiple transactions
class block
{
	// String hasher to generate a signature
	hash<string> hasher;

	// Hashing function
	int GetHash(int piPreviousHash, vector <string>& psTransactions)
	{
		// Compute the string hash
		return  hasher(accumulate(psTransactions.begin(),psTransactions.end(),to_string(piPreviousHash)));
	}

	// Member variables: Previous hash, current hash and transaction collection
	int iPreviousHash;
	vector<string> sTransactions;
	int iCurrentHash;

public:
        // Disable default constructor
        block () = delete;

	// Constructor: Previous hash and transactions
	block (int pPreviousHash, vector<string>& psTransactions) : iPreviousHash(pPreviousHash), sTransactions(psTransactions), iCurrentHash(GetHash(iPreviousHash,sTransactions))
	{
	}

	// Accessors to block parameters
	int PreviousHash() const { return iPreviousHash; }
	int CurrentHash() const { return iCurrentHash; }
	const vector<string> Transactions() const { return sTransactions; }
};

// Class that chains all blocks
class block_chain
{
	// Collection of blocks in chain
	vector<block> chain;

	// Declare allowable coins and sides
	vector<string> const sSides {"B","S"};
	vector<string> const sCoins {"BTC","LTC","ETH","BCH","ETC","XVG","XRP","XLM","EOS","BSV","TRX","ADA","XMR","NEM","DASH","NEO"};
        int const num_coins = sCoins.size()-1;

public:

	// Constructor
	block_chain() = default;

	// Accessor to blocks
	vector<block> GetBlocks() 
	{
		return chain;
	}

	// Find all blocks of a chain starting at its head id
	static vector<block> find(int header_id, vector<block> known_blocks) 
	{
		// Declare the result set
		vector<block> chainblocks;

		// Find the head block per its header id
		auto foundblock = find_if(known_blocks.begin(), known_blocks.end(),

							[&header_id] (block &b) -> bool
							{
								return b.CurrentHash() == header_id;
							}
		);

		// Iterate over all blocks of the chain
		while (foundblock != known_blocks.end()) 
		{
			// Push this block to the chain
			chainblocks.push_back(*foundblock);

			// Find the next block in chain. Match the found previous id to the candidate current id
			foundblock = find_if(known_blocks.begin(), known_blocks.end(),

							[&foundblock](block &b) -> bool
							{
								return b.CurrentHash() == (*foundblock).PreviousHash(); 
							}
			);
		}
		
		// Retun the result set
		return chainblocks;
	}

	// Add transactions to a new block and add the block to the chain
	int Add(vector<string>&& pvTransactions) 
	{
		// Get previous hash
		int iPreviousHash {-1};
		if (!chain.empty()) 
                { 
                   iPreviousHash = chain.back().CurrentHash(); 
                }

		// Allocate a new block with the new transactions
                block oNewBlock {iPreviousHash, pvTransactions};

		// Save the block to the chain
		chain.push_back(oNewBlock);

		// Return the hash of the new block
		return oNewBlock.CurrentHash();
	}

        // Generate random transactions for a block
        vector<string> GenerateRandomTransactions(int maxtransactions = 6) 
        {
		// Declare result set
		vector<string> transactions;

		// Declare random devices
		random_device rd;
		mt19937 mt(rd());
	
		// Generate a random number of transactions per block
		uniform_int_distribution<int> num_transact_dist(2,max(2,maxtransactions));
 
		// Iterate over all transactions in a block
		for (int i=1; i < num_transact_dist(mt); i++) 
		{
	     		// Create distrubutions for transactions
	     		uniform_int_distribution<int> size_dist(1,100);
	     		uniform_int_distribution<int> coin_dist(0,num_coins);
	     		uniform_int_distribution<int> side_dist(0,1);

	     		// Add transaction to the result set
	     		transactions.push_back(sSides[side_dist(mt)] + " " + to_string(size_dist(mt)) + " " + sCoins[coin_dist(mt)]); 
		}

		// Return the result set
		return transactions;
        }

	// Create a block and add transactions
	int AddTransactions(int num_transactions) 
	{
	     return Add(GenerateRandomTransactions(num_transactions));
 	}	

	// Print the current contents of the chain
	void Print() 
	{
		// Printing block chain (starting with the genesis block)
		cout << "\nBlockchain\t[CurrentID\t| PreviousID \t| Transactions]\n\n";

		// Declare block counter
		int i {0};

		// Iterate over all blocks in the chain
		for (auto & c : chain) 
		{
			// Get all transactions for this current block
			string sTrans;

			// Concatenate all transactions for this block
			for (auto& x : c.Transactions()) { sTrans += x + ", "; }

			// Right trim the last separator (comma)
			sTrans.erase(sTrans.find_last_of (','),2);

			// Print the contents of this block
		        cout << "Block " << ++i << ": " << hex << uppercase << setw(8) << "0x" << c.CurrentHash() << "\t| 0x" << c.PreviousHash() << "\t| " << sTrans << "\n"; 
		}
	}

	// Print the current contents of the chain
	static void Print(const vector<block> &blockchain)
	{
		// Printing block vector (including complete blockchain - starting with latest block)
		cout << "\nBlock vector [CurrentID\t| PreviousID \t| Transactions]\n\n";

		// Iterate over the parameters of the block chain
		for (auto &b : blockchain) 
		{
			// Get all transactions for this current block
			string sTrans;;

			// Concatenate all transactions for this block
			for (auto& x : b.Transactions()) { sTrans += x + ", "; }

			// Right trim the last separator (comma)
			sTrans.erase(sTrans.find_last_of (','),2);

			// Print the contents of this block
			cout << "Block: " << hex << uppercase << setw(8) << "0x" << b.CurrentHash() << "\t| 0x" << b.PreviousHash() << "\t| " << sTrans << "\n"; 
		}
	}
};

// -------------
// Main routine
// -------------
int main (int argc, char** argv)
{
   try {
	// Declare new block chain
	block_chain bc;

        // Get the number of blocks in chain 
	int num_blocks = max(10, argc < 2 ? 10 : atoi(argv[1]));

        // Get the number of transactions per block
        int num_transactions = max (6, argc > 2 ? atoi(argv[2]) : 6); 

	// Generate transactions for the number of input blocks
        for (int i=0; i < num_blocks; i++) 
	{
	     bc.AddTransactions(num_transactions);
	} 

	// Print the current contents of the block chain
	bc.Print();
	
	// Find block chain by starting with a previously known header id 
	vector<block> found_chain_by_header_id = bc.find (bc.GetBlocks().back().CurrentHash(), bc.GetBlocks());

	// Print blocks per the chain found in the previous find
	bc.Print(found_chain_by_header_id);
   }
   catch (...) 
   {
	return -1;
   }

   // Exit the program
   return 0;
}
