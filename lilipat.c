#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 255
#define MAX_ITEMS 100
#define MAX_BUDGETS 12

typedef struct {
    char name[MAX_LENGTH];
    char brand[MAX_LENGTH];
    int price;
    char purchase_link[MAX_LENGTH];
    char category[MAX_LENGTH];
    time_t timestamp;
    int id;
    int budget_month;
} Item;

typedef struct {
    int month;
    int budget;
    int remaining;
    char item_ids[MAX_ITEMS][10];
    int item_count;
} Budget;

/*
Description: Generates a random unique ID for an item.
Parameters: None
Returns: An integer representing a randomly generated ID.
*/
int generate_random_id() {
    return rand() % 1000000 + 1;
}

/*
Description: Validates if a given URL starts with "http://" or "https://" and contains a valid domain.
Parameters: url - The URL string to validate.
Returns: 1 if valid, 0 otherwise.
*/
int is_valid_url(const char *url) {
    return (strncmp(url, "http://", 7) == 0 || strncmp(url, "https://", 8) == 0) &&
           (strstr(url, ".com") || strstr(url, ".org") || strstr(url, ".net"));
}

/*
Description: Checks if the given category matches predefined valid categories.
Parameters: category - The category string to validate.
Returns: 1 if valid, 0 otherwise.
*/
int is_valid_category(const char *category) {
    const char *valid_categories[] = {"furniture", "electronics", "appliances", "bedroom", "bathroom", 
                                       "living room", "dining room", "office", "outdoor", "miscellaneous"};
    for (int i = 0; i < 10; i++) {
        if (strcmp(category, valid_categories[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
Description: Saves an item to the "items.txt" file.
Parameters: item - Pointer to the Item structure to be saved.
Returns: None.
*/
void save_item_to_file(const Item *item) {
    FILE *file = fopen("items.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(file, "%d,%s,%s,%d,%s,%s,%ld\n", item->id, item->name, item->brand, item->price, item->purchase_link, item->category, item->timestamp);
    fclose(file);
}

/*
Description: Loads items from the "items.txt" file into an array.
Parameters:
items[] - Array to store loaded items.
count - Pointer to an integer to store the number of loaded items.
Returns: None.
*/
void load_items(Item items[], int *count) {
    FILE *file = fopen("items.txt", "r");
    if (file == NULL) {
        printf("No existing items found.\n");
        return;
    }
    *count = 0;
    while (fscanf(file, "%d,%[^,],%[^,],%d,%[^,],%[^,],%ld\n", &items[*count].id, items[*count].name, items[*count].brand, &items[*count].price, items[*count].purchase_link, items[*count].category, &items[*count].timestamp) != EOF) {
        (*count)++;
    }
    fclose(file);
}

/*
Description: Saves all items from the array to "items.txt" (overwrites the file).
Parameters:
items[] - Array of items to be saved.
count - Number of items in the array.
Returns: None.
*/
void save_items(Item items[], int count) {
    FILE *file = fopen("items.txt", "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%s,%d,%s,%s,%ld\n", items[i].id, items[i].name, items[i].brand, items[i].price, items[i].purchase_link, items[i].category, items[i].timestamp);
    }
    fclose(file);
}

/*
Description: Prompts the user to enter item details and saves the item.
Parameters: None.
Returns: None.
*/
void addItem(){
	Item item;
    char temp_price[MAX_LENGTH];

    printf("Enter Item Name: ");
    fgets(item.name, MAX_LENGTH, stdin);
    item.name[strcspn(item.name, "\n")] = 0;
    if (strlen(item.name) == 0) {
        printf("Item name cannot be empty!\n");
        return;
    }

    printf("Enter Item Brand: ");
    fgets(item.brand, MAX_LENGTH, stdin);
    item.brand[strcspn(item.brand, "\n")] = 0;

    printf("Enter Item Price: ");
    fgets(temp_price, MAX_LENGTH, stdin);
    float price_float = atof(temp_price);
    item.price = (int)(price_float * 100);

    printf("Enter Purchase Link: ");
    fgets(item.purchase_link, MAX_LENGTH, stdin);
    item.purchase_link[strcspn(item.purchase_link, "\n")] = 0;
    if (!is_valid_url(item.purchase_link)) {
        printf("Invalid purchase link!\n");
        return;
    }

    printf("Enter Category: ");
    fgets(item.category, MAX_LENGTH, stdin);
    item.category[strcspn(item.category, "\n")] = 0;
    if (!is_valid_category(item.category)) {
        printf("Invalid category!\n");
        return;
    }

    item.timestamp = time(NULL);
    item.id = generate_random_id();

    save_item_to_file(&item);
    printf("Item added successfully! ID: %d\n", item.id);
}

/*
Description: Displays the list of items and allows the user to remove one.
Parameters: None.
Returns: None.
*/
void removeItem(){
    Item items[MAX_ITEMS];
    int count;
    load_items(items, &count);
    
    if (count == 0) {
        printf("No items to remove.\n");
        return;
    }
    
    printf("Items to remove:\n");
    for (int i = count - 1; i >= 0; i--) {
        printf("[%d] %s (%s, %s) - %d\n    %s\n", count - i, items[i].name, items[i].brand, items[i].category, items[i].price, items[i].purchase_link);
    }
    printf("[x] Back\n\n");
    
    printf("Select item to remove: ");
    char input[MAX_LENGTH];
    fgets(input, MAX_LENGTH, stdin);
    if (input[0] == 'x' || input[0] == 'X') return;
    
    int selection = atoi(input);
    if (selection < 1 || selection > count) {
        printf("Invalid selection!\n");
        return;
    }
    
    selection = count - selection;
    for (int i = selection; i < count - 1; i++) {
        items[i] = items[i + 1];
    }
    count--;
    save_items(items, count);
    printf("Item removed successfully!\n");
}

/*
Description: Allows the user to set a monthly budget and assign items to it.
Parameters:
budgets - Array of budget structures.
budget_count - Pointer to the number of budgets.
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void setBudget(Budget *budgets, int *budget_count, Item *items, int item_count) {
    if (*budget_count >= MAX_BUDGETS) {
        printf("Budget list is full!\n");
        return;
    }

    int month;
    printf("Enter month (1-12): ");
    scanf("%d", &month);

    for (int i = 0; i < *budget_count; i++) {
        if (budgets[i].month == month) {
            printf("Budget for this month is already set.\n");
            return;
        }
    }

    int budget;
    printf("Enter budget for %s: ", (char *[]){"January", "February", "March", "April", "May", "June",
                                               "July", "August", "September", "October", "November", "December"}[month - 1]);
    scanf("%d", &budget);

    Budget newBudget;
    newBudget.month = month;
    newBudget.budget = budget;
    newBudget.remaining = budget;
    newBudget.item_count = 0;

    printf("\nUnbudgeted Items:\n");
    int available_items[MAX_ITEMS], available_count = 0;
    for (int i = 0; i < item_count; i++) {
        if (items[i].budget_month == 0) {
            printf("[%d] %s (%s, %s) - %.2f\n", available_count + 1, items[i].name, items[i].brand, items[i].category, items[i].price / 100.0);
            available_items[available_count++] = i;
        }
    }

    while (available_count > 0) {
        int choice;
        printf("Select item to add to %s's budget (or 0 to finish): ", (char *[]){"January", "February", "March", "April", "May", "June",
                                                                                "July", "August", "September", "October", "November", "December"}[month - 1]);
        scanf("%d", &choice);

        if (choice == 0) break;

        if (choice > 0 && choice <= available_count) {
            int itemIndex = available_items[choice - 1];

            if (newBudget.remaining >= items[itemIndex].price) {
                strcpy(newBudget.item_ids[newBudget.item_count], items[itemIndex].id);
                newBudget.item_count++;
                newBudget.remaining -= items[itemIndex].price;
                items[itemIndex].budget_month = month;
                printf("Item added to budget!\n");
            } else {
                printf("Not enough budget for this item.\n");
            }
        } else {
            printf("Invalid choice, try again.\n");
        }
    }

    budgets[*budget_count] = newBudget;
    (*budget_count)++;
    printf("Budget set successfully!\n");
}

/*
Description: Displays details of the budget for a specific month.
Parameters:
budgets - Array of budget structures.
budget_count - Number of budgets.
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void viewBudget(Budget *budgets, int budget_count, Item *items, int item_count) {
    if (budget_count == 0) {
        printf("No budgets set yet.\n");
        return;
    }

    int month;
    printf("Enter month to view budget (1-12): ");
    scanf("%d", &month);

    int found = 0;
    for (int i = 0; i < budget_count; i++) {
        if (budgets[i].month == month) {
            found = 1;
            printf("\nBudget for %s:\n", (char *[]){"January", "February", "March", "April", "May", "June",
                                                      "July", "August", "September", "October", "November", "December"}[month - 1]);
            printf("Total Budget: %d\n", budgets[i].budget);
            printf("Remaining Budget: %d\n", budgets[i].remaining);
            printf("Items in Budget:\n");

            for (int j = 0; j < budgets[i].item_count; j++) {
                for (int k = 0; k < item_count; k++) {
                    if (strcmp(budgets[i].item_ids[j], items[k].id) == 0) {
                        printf("- %s (%s, %s) - %.2f\n", items[k].name, items[k].brand, items[k].category, items[k].price / 100.0);
                        break;
                    }
                }
            }
            break;
        }
    }

    if (!found) {
        printf("No budget found for this month.\n");
    }
}

/*
Description: Allows the user to remove a budget and unassign its items.
Parameters:
budgets - Array of budget structures.
budget_count - Pointer to the number of budgets.
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void removeBudget(Budget *budgets, int *budget_count, Item *items, int item_count) {
    if (*budget_count == 0) {
        printf("No budgets to remove.\n");
        return;
    }

    printf("Available budgets to remove:\n");
    for (int i = 0; i < *budget_count; i++) {
        printf("[%d] %s\n", i + 1, (char *[]){"January", "February", "March", "April", "May", "June",
                                               "July", "August", "September", "October", "November", "December"}[budgets[i].month - 1]);
    }

    int choice;
    printf("Select a budget to remove (or 0 to cancel): ");
    scanf("%d", &choice);

    if (choice == 0) return;

    if (choice > 0 && choice <= *budget_count) {
        int month = budgets[choice - 1].month;

        for (int i = choice - 1; i < *budget_count - 1; i++) {
            budgets[i] = budgets[i + 1];
        }
        (*budget_count)--;

        for (int i = 0; i < item_count; i++) {
            if (items[i].budget_month == month) {
                items[i].budget_month = 0;
            }
        }

        printf("Budget removed successfully!\n");
    } else {
        printf("Invalid choice, try again.\n");
    }
}

/*
Description: Displays a summary menu to show items and budget details.
Parameters:
items - Array of items.
item_count - Number of items.
budgets - Array of budgets.
budget_count - Number of budgets.
Returns: None.
*/
void summarize(Item items[], int item_count, Budget budgets[], int budget_count) {
    char choice;
    do {
        printf("\nSummarize\n");
        printf("[1] Items added\n");
        printf("[2] Budget summary\n");
        printf("[x] Back\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);
        getchar();

        switch (choice) {
            case '1':
                summarizeItems(items, item_count);
                break;
            case '2':
                summarizeBudget(budgets, budget_count, items, item_count);
                break;
            case 'x':
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 'x');
}

/*
Description: Displays the list of added items with sorting options.
Parameters:
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void summarizeItems(Item items[], int item_count) {
    char choice;
    int sortType = 1;
    int ascending = 1;

    do {
        sortItems(items, item_count, sortType, ascending);

        printf("\nItems added:\n");
        for (int i = 0; i < item_count; i++) {
            printf("%s (%s, %s) - %.2f\n  %s\n",
                   items[i].name, items[i].brand, items[i].category,
                   items[i].price / 100.0, items[i].purchase_link);
            if (items[i].budget_month != 0) {
                printf("  To be purchased on: %s\n", 
                       (char *[]){"January", "February", "March", "April", "May", "June",
                                  "July", "August", "September", "October", "November", "December"}[items[i].budget_month - 1]);
            } else {
                printf("  Not yet budgeted\n");
            }
        }

        printf("\n[q] Sort by date added\n[w] Sort by price\n[e] Sort by name\n[x] Back\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);
        getchar();

        if (choice == 'q') {
            sortType = 2;
            ascending *= -1;
        } else if (choice == 'w') {
            sortType = 3;
            ascending *= -1;
        } else if (choice == 'e') {
            sortType = 1;
            ascending *= -1;
        }

    } while (choice != 'x');
}

/*
Description: Sorts the list of items based on name, date, or price.
Parameters:
items - Array of items.
count - Number of items.
type - Sorting type (1: Name, 2: Date, 3: Price).
ascending - Sorting order (1: Ascending, -1: Descending).
Returns: None.
*/
void sortItems(Item items[], int count, int type, int ascending) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            int swap = 0;
            if (type == 1) { // Sort by name
                if ((ascending == 1 && strcmp(items[i].name, items[j].name) > 0) ||
                    (ascending == -1 && strcmp(items[i].name, items[j].name) < 0)) {
                    swap = 1;
                }
            } else if (type == 2) { // Sort by date
                if ((ascending == 1 && items[i].timestamp > items[j].timestamp) ||
                    (ascending == -1 && items[i].timestamp < items[j].timestamp)) {
                    swap = 1;
                }
            } else if (type == 3) { // Sort by price
                if ((ascending == 1 && items[i].price > items[j].price) ||
                    (ascending == -1 && items[i].price < items[j].price)) {
                    swap = 1;
                }
            }

            if (swap) {
                Item temp = items[i];
                items[i] = items[j];
                items[j] = temp;
            }
        }
    }
}

/*
Description: Displays a summary of the total budget, remaining budget, and number of items per month.
Parameters:
budgets - Array of budgets.
budget_count - Number of budgets.
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void summarizeBudget(Budget budgets[], int budget_count, Item items[], int item_count) {
    char choice;
    do {
        printf("\nBudget Summary:\n");
        for (int i = 0; i < budget_count; i++) {
            int totalSpent = budgets[i].budget - budgets[i].remaining;
            printf("%s:\n  Total budget: %d\n  Remaining after purchases: %d\n  %d item/s to purchase.\n\n",
                   (char *[]){"January", "February", "March", "April", "May", "June",
                              "July", "August", "September", "October", "November", "December"}[budgets[i].month - 1],
                   budgets[i].budget, budgets[i].remaining, budgets[i].item_count);
        }

        printf("[v] View budget details\n[x] Back\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);
        getchar();

        if (choice == 'v') {
            viewBudgetDetails(budgets, budget_count, items, item_count);
        }
    } while (choice != 'x');
}

/*
Description: Displays detailed budget information for a selected month.
Parameters:
budgets - Array of budgets.
budget_count - Number of budgets.
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void viewBudgetDetails(Budget budgets[], int budget_count, Item items[], int item_count) {
    int month;
    printf("Enter month to view details (1-12): ");
    scanf("%d", &month);
    getchar();

    for (int i = 0; i < budget_count; i++) {
        if (budgets[i].month == month) {
            printf("\nBudget for %s:\n", (char *[]){"January", "February", "March", "April", "May", "June",
                                                     "July", "August", "September", "October", "November", "December"}[month - 1]);
            printf("Total Budget: %d\n", budgets[i].budget);
            printf("Remaining Budget: %d\n", budgets[i].remaining);
            printf("Items in Budget:\n");

            for (int j = 0; j < budgets[i].item_count; j++) {
                for (int k = 0; k < item_count; k++) {
                    if (budgets[i].item_ids[j] == items[k].id) {
                        printf("- %s (%s, %s) - %.2f\n", 
                               items[k].name, items[k].brand, items[k].category, items[k].price / 100.0);
                        break;
                    }
                }
            }
            return;
        }
    }

    printf("No budget found for this month.\n");
}

/*
Description: Displays the menu for adding and removing items.
Parameters: None.
Returns: None.
*/
void displayMenu1() {
    printf("\nAdd Item Menu\n");
    printf("[1] Add item to purchase\n");
    printf("[2] Remove item from purchase\n");
    printf("[x] Back\n");
}

/*
Description: Handles the add/remove item menu loop.
Parameters: None.
Returns: None.
*/
void addItemMenu() {
    char choice;
    do {
        displayMenu1();
        printf("\nEnter your choice: ");
        scanf(" %c", &choice);
        getchar();

        switch (choice) {
            case '1':
                addItem();
                break;
            case '2':
                removeItem();
                break;
            case 'x':
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 'x');
}

/*
Description: Displays the menu for budget management.
Parameters: None.
Returns: None.
*/
void displayMenu2() {
    printf("\nBudget items for Purchase\n");
    printf("[1] Set budget\n");
    printf("[2] View budget\n");
    printf("[3] Remove month from budget\n");
    printf("[x] Back\n");
}

/*
Description: Handles budget-related operations, including setting, viewing, and removing budgets.
Parameters:
budgets - Array of budgets.
budget_count - Pointer to the number of budgets.
items - Array of items.
item_count - Number of items.
Returns: None.
*/
void budgetItems(Budget budgets[], int *budget_count, Item items[], int item_count) {
    char choice;
    do {
        displayMenu2();
        printf("\nEnter your choice: ");
        scanf(" %c", &choice);
        getchar();

        switch (choice) {
            case '1':
                setBudget(budgets, budget_count, items, item_count);
                break;
            case '2':
                viewBudget(budgets, *budget_count, items, item_count);
                break;
            case '3':
                removeBudget(budgets, budget_count, items, item_count);
                break;
            case 'x':
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 'x');
}

/*
Description: Displays the main menu.
Parameters: None.
Returns: None.
*/
void displayMenu() {
    printf("\nMain Menu\n");
    printf("[1] Add item\n");
    printf("[2] Budget items for purchase\n");
    printf("[3] Summarize\n");
    printf("[x] Exit\n");
}

int main() {
	srand(time(NULL));
    Budget budgets[MAX_ITEMS];
    int budget_count = 0;
    Item items[MAX_ITEMS];
    int item_count = 0;
    char choice;
    
    printf("Welcome to Lilipat!\n");
    
    do {
        displayMenu();
        printf("\nEnter your choice: ");
        scanf(" %c", &choice);
        getchar();
        
        switch (choice) {
            case '1':
                addItemMenu();
                break;
            case '2':
                budgetItems(budgets, &budget_count, items, item_count);
                break;
            case '3':
                summarize(items, item_count, budgets, budget_count);
                break;
            case 'x':
            case 'X':
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 'x' && choice != 'X');
    
    return 0;
}
