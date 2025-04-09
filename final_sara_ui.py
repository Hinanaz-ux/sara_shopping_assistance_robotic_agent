import tkinter as tk

# Shopping cart dictionary
shopping_cart = {}
sorted_order = []  # Store the sorted order here

item_mapping = {"0": "Pizza", "1": "Clock", "2": "Broccoli", "3": "Apple"}
2
def add_to_cart(item_id):
    item_name = item_mapping[item_id]
    shopping_cart[item_id] = item_name
    print(f"Added item {item_name} to cart. Current cart: {shopping_cart}")
    update_cart_display()

def remove_from_cart():
    selected_index = cart_list.curselection()
    if selected_index:
        selected_item = cart_list.get(selected_index)
        item_id = [k for k, v in shopping_cart.items() if v == selected_item][0]
        del shopping_cart[item_id]
        print(f"Removed item {selected_item} from cart. Current cart: {shopping_cart}")
        update_cart_display()

def place_order():
    global sorted_order
    sorted_order = sorted(map(int, shopping_cart.keys()))
    print(f"Order placed: {sorted_order}")
    shopping_cart.clear()
    update_cart_display()

def show_cart_page():
    item_selection_frame.pack_forget()
    cart_frame.pack()
    update_cart_display()

def show_item_selection_page():
    cart_frame.pack_forget()
    item_selection_frame.pack()

def update_cart_display():
    cart_list.delete(0, tk.END)
    for item in shopping_cart.values():
        cart_list.insert(tk.END, item)

# Create main window
root = tk.Tk()
root.title("SARA: Shopping Assistant Robot Agent")
root.geometry("400x400")

# Item selection page
item_selection_frame = tk.Frame(root)
tk.Label(item_selection_frame, text="Select Items", font=("Arial", 30)).pack()
btn1 = tk.Button(item_selection_frame, text="Pizza", command=lambda: add_to_cart("0"))
btn2 = tk.Button(item_selection_frame, text="Clock", command=lambda: add_to_cart("1"))
btn3 = tk.Button(item_selection_frame, text="Broccoli", command=lambda: add_to_cart("2"))
btn4 = tk.Button(item_selection_frame, text="Apple", command=lambda: add_to_cart("3"))
btn_cart = tk.Button(item_selection_frame, text="View Cart", command=show_cart_page)

btn1.pack(pady=5)
btn2.pack(pady=5)
btn3.pack(pady=5)
btn4.pack(pady=5)
btn_cart.pack(pady=5)

# Cart page
cart_frame = tk.Frame(root)
tk.Label(cart_frame, text="Shopping Cart", font=("Arial", 14)).pack()
cart_list = tk.Listbox(cart_frame, height=10, width=40)
cart_list.pack()
btn_remove = tk.Button(cart_frame, text="Remove Selected Item", command=remove_from_cart)
btn_order = tk.Button(cart_frame, text="Place Order", command=place_order)
btn_back = tk.Button(cart_frame, text="Back to Items", command=show_item_selection_page)

btn_remove.pack(pady=5)
btn_order.pack(pady=5)
btn_back.pack(pady=5)

# Start on item selection page
item_selection_frame.pack()

# Run the application
root.mainloop()
