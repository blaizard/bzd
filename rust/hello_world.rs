use non_owning_ring_spin::{NonOwningRingSpin, NonOwningRingSpinElement};
use std::cell::RefCell;
use std::rc::Rc;

fn main() {
    let mut ring = NonOwningRingSpin::new();

    // Create elements
    let elem1 = Rc::new(RefCell::new(NonOwningRingSpinElement::new()));
    let elem2 = Rc::new(RefCell::new(NonOwningRingSpinElement::new()));
    let elem3 = Rc::new(RefCell::new(NonOwningRingSpinElement::new()));

    // Push elements to the ring buffer
    ring.push_back(elem1.clone());
    ring.push_back(elem2.clone());
    ring.push_back(elem3.clone());

    // Pop front element
    if let Some(popped) = ring.pop_front_any() {
        println!("Popped an element: {:?}", Rc::as_ptr(&popped));
    }

    // Pop another element with a predicate (e.g., pop only if not detached)
    if let Some(popped) = ring.pop_front(|e| !e.is_detached()) {
        println!(
            "Popped an element with predicate: {:?}",
            Rc::as_ptr(&popped)
        );
    }

    // Clear the ring buffer
    ring.clear();
    println!("Ring buffer cleared!");
}
