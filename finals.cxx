#include <iostream>
#include <vector>
#include <string>
#include <iomanip> // For formatting output
#include <limits>  // For numeric_limits

// --- 1. Item Structure and Inventory System ---

struct MenuItem {
    std::string name;
    double price;
    int quantity; // Simple inventory tracking
    int max_stock; // Maximum capacity for restocking
};

// Function to initialize the menu/inventory
std::vector<MenuItem> initializeMenu() {
    return {
        // Hotdogs (Name, Price, Current Stock, Max Stock)
        {"Classic Hotdog", 30.00, 50, 100},
        {"Chili Cheese Dog", 50.00, 30, 50},
        {"Chicken Hotdog", 45.00, 20, 40},

        // Drinks (Name, Price, Current Stock, Max Stock)
        {"Soda (Can)", 36.00, 100, 200},
        {"Bottled Water", 15.00, 80, 150},
        {"Lemonade", 20.00, 40, 70}
    };
}

// Function to safely handle integer input
int getSafeIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            // Check for negative numbers (unless specific prompt allows them)
            if (value >= 0) {
                return value;
            } else {
                std::cout << "Input must be a non-negative number." << std::endl;
            }
        } else {
            std::cout << "Invalid input. Please enter a whole number." << std::endl;
            std::cin.clear(); // Clear the error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
        }
    }
}

// Function to display the menu
void displayMenu(const std::vector<MenuItem>& menu) {
    std::cout << "\n======================================" << std::endl;
    std::cout << "        Mainit na Aso's Hotdog Menu" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::left << std::setw(5) << "Item Id"
              << std::setw(25) << "Item"
              << std::setw(10) << "Price"
              << "Stocks" << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    for (size_t i = 0; i < menu.size(); ++i) {
        // Only show items that are in stock for ordering, but show all for restock view
        // For the main menu, let's keep the original logic: only show if > 0
        if (menu[i].quantity > 0) {
            std::cout << std::left << std::setw(5) << i + 1 // ID starts at 1
                      << std::setw(25) << menu[i].name
                      << "$" << std::fixed << std::setprecision(2) << std::setw(8) << menu[i].price
                      << menu[i].quantity << std::endl;
        }
    }
    std::cout << "======================================" << std::endl;
}

// Function to restock the inventory
void restockInventory(std::vector<MenuItem>& menu) {
    std::cout << "\n*** INVENTORY RESTOCK MODE ***" << std::endl;

    for (size_t i = 0; i < menu.size(); ++i) {
        int currentStock = menu[i].quantity;
        int maxStock = menu[i].max_stock;
        int needed = maxStock - currentStock;

        std::cout << "\nCode " << i + 1 << ": " << menu[i].name
                  << " | Current Stock: " << currentStock
                  << " | Max Capacity: " << maxStock << std::endl;

        if (needed <= 0) {
            std::cout << "  -> Stock is full." << std::endl;
            continue;
        }

        std::cout << "  -> Recommended Restock: " << needed << std::endl;
        
        int restockAmount = getSafeIntInput("  Enter amount to add (0 to skip): ");

        if (restockAmount > 0) {
            // Prevent overstocking beyond max_stock
            if (currentStock + restockAmount > maxStock) {
                restockAmount = maxStock - currentStock;
                std::cout << "  Warning: Can only add " << restockAmount << " to reach max capacity." << std::endl;
            }
            
            menu[i].quantity += restockAmount;
            std::cout << "  **Successfully restocked " << restockAmount << " units.** New stock: " << menu[i].quantity << std::endl;
        }
    }
    std::cout << "\n*** RESTOCK COMPLETE ***" << std::endl;
}

// Function to process the order
void processOrder(std::vector<MenuItem>& menu) {
    std::vector<std::pair<MenuItem, int>> cart; // Stores {item, quantity}
    double totalCost = 0.0;
    int choice = -1;
    int quantity = 0;

    std::cout << "\nWelcome to Mainit na Aso's! Start your order (Type '0' to finish order)." << std::endl;

    while (true) {
        displayMenu(menu);

        // Use the safe input function for choice
        choice = getSafeIntInput("\nEnter Item ID (1-" + std::to_string(menu.size()) + ") or '0' to checkout: ");

        if (choice == 0) {
            break; // Exit the loop to proceed to checkout
        }

        // Validate item ID
        if (choice < 1 || choice > menu.size()) {
            std::cout << "Invalid item ID. Please try again." << std::endl;
            continue;
        }

        // Map 1-based choice back to 0-based index
        int itemIndex = choice - 1;

        // Check if the item is in stock
        if (menu[itemIndex].quantity <= 0) {
            std::cout << "Sorry, " << menu[itemIndex].name << " is currently out of stock!" << std::endl;
            continue;
        }

        // Get quantity safely
        std::string qty_prompt = "How many " + menu[itemIndex].name + "s do you want? (Max " + std::to_string(menu[itemIndex].quantity) + "): ";
        quantity = getSafeIntInput(qty_prompt);

        // Validate quantity
        if (quantity <= 0) {
            std::cout << "Quantity must be greater than zero." << std::endl;
            continue;
        }
        if (quantity > menu[itemIndex].quantity) {
            std::cout << "Only " << menu[itemIndex].quantity << " are in stock. Adding all available." << std::endl;
            quantity = menu[itemIndex].quantity;
        }

        // Update inventory and cart
        menu[itemIndex].quantity -= quantity; // Decrement inventory
        
        // IMPORTANT: Copy the menu item details for the receipt (before decrementing current inventory)
        MenuItem orderedItem = menu[itemIndex];
        orderedItem.quantity = quantity; // Set the quantity to the ordered amount for the cart entry
        
        cart.push_back({orderedItem, quantity});
        double itemCost = orderedItem.price * quantity;
        totalCost += itemCost;

        std::cout << "\nAdded " << quantity << " x " << orderedItem.name
                  << " to your order. Current total: $"
                  << std::fixed << std::setprecision(2) << totalCost << std::endl;
    }

    // --- Checkout Process ---
    std::cout << "\n\n**************************************" << std::endl;
    std::cout << "             ORDER RECEIPT" << std::endl;
    std::cout << "**************************************" << std::endl;

    if (cart.empty()) {
        std::cout << "You didn't order anything. Order canceled." << std::endl;
        std::cout << "**************************************" << std::endl;
        return;
    }

    // Display cart contents
    for (const auto& item : cart) {
        // item.first is the MenuItem struct, item.second is the quantity
        std::cout << std::left << std::setw(5) << item.second << " x "
                  << std::setw(25) << item.first.name
                  << " $" << std::fixed << std::setprecision(2) << (item.first.price * item.second) << std::endl;
    }

    std::cout << "--------------------------------------" << std::endl;
    std::cout << std::left << std::setw(31) << "Subtotal:"
              << " $" << std::fixed << std::setprecision(2) << totalCost << std::endl;

    // A simple sales tax calculation
    const double TAX_RATE = 0.07; // 7% tax
    double taxAmount = totalCost * TAX_RATE;
    double grandTotal = totalCost + taxAmount;

    std::cout << std::left << std::setw(31) << "Tax (7%):"
              << " $" << std::fixed << std::setprecision(2) << taxAmount << std::endl;

    std::cout << "**************************************" << std::endl;
    std::cout << std::left << std::setw(31) << "** GRAND TOTAL **:"
              << " ** $" << std::fixed << std::setprecision(2) << grandTotal << " **" << std::endl;
    std::cout << "**************************************" << std::endl;
    std::cout << "Thank you for visiting Mainit na Aso's Hotdog!" << std::endl;
}

// --- 3. Main Function (Continuous Loop) ---
int main() {
    // 1. Initialize the inventory
    std::vector<MenuItem> menu = initializeMenu();
    char mainChoice;

    // Set output formatting once
    std::cout << std::fixed << std::setprecision(2);

    while (true) {
        std::cout << "\n\n=== Mainit na Aso's System Menu ===" << std::endl;
        std::cout << "[O]rder | [R]estock | [E]xit" << std::endl;
        std::cout << "Enter choice: ";
        
        if (!(std::cin >> mainChoice)) {
            std::cout << "Invalid input. Please try again." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        // Convert input to uppercase for easier comparison
        mainChoice = std::toupper(mainChoice);

        switch (mainChoice) {
            case 'O':
                processOrder(menu);
                break;
            case 'R':
                restockInventory(menu);
                break;
            case 'E':
                std::cout << "\nExiting Mainit na Aso's System. Goodbye!" << std::endl;
                return 0; // Exit the main function and the program
            default:
                std::cout << "Invalid option. Please choose 'O', 'R', or 'E'." << std::endl;
                break;
        }
    }

    return 0;
}
