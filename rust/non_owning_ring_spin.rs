use std::cell::RefCell;
use std::rc::Rc;
use std::sync::Mutex;

/// A non-owning ring element
pub struct NonOwningRingSpinElement {
    next: Option<Rc<RefCell<NonOwningRingSpinElement>>>,
}

impl Default for NonOwningRingSpinElement {
    fn default() -> Self {
        NonOwningRingSpinElement::new()
    }
}

impl NonOwningRingSpinElement {
    pub fn new() -> Self {
        Self { next: None }
    }

    pub fn is_detached(&self) -> bool {
        self.next.is_none()
    }
}

/// A non-owning ring spin queue
pub struct NonOwningRingSpin {
    dummy: Rc<RefCell<NonOwningRingSpinElement>>,
    head: Rc<RefCell<NonOwningRingSpinElement>>,
    mutex: Mutex<()>,
}

impl Default for NonOwningRingSpin {
    fn default() -> Self {
        NonOwningRingSpin::new()
    }
}

impl NonOwningRingSpin {
    pub fn new() -> Self {
        let dummy = Rc::new(RefCell::new(NonOwningRingSpinElement::new()));
        dummy.borrow_mut().next = Some(dummy.clone());
        Self {
            dummy: dummy.clone(),
            head: dummy.clone(),
            mutex: Mutex::new(()),
        }
    }

    /// Push an element at the back of the ring
    pub fn push_back(&mut self, element: Rc<RefCell<NonOwningRingSpinElement>>) {
        let _lock = self.mutex.lock().unwrap();
        element.borrow_mut().next = self.head.borrow().next.clone();
        self.head.borrow_mut().next = Some(element.clone());
        self.head = element;
    }

    /// Pop an element from the front based on a predicate
    pub fn pop_front<F>(&mut self, predicate: F) -> Option<Rc<RefCell<NonOwningRingSpinElement>>>
    where
        F: Fn(&NonOwningRingSpinElement) -> bool,
    {
        let _lock = self.mutex.lock().unwrap();
        let original_head = self.head.clone();
        loop {
            let element = self.head.borrow().next.clone();
            if let Some(ref elem) = element {
                if Rc::ptr_eq(elem, &self.dummy) || !predicate(&elem.borrow()) {
                    self.head = elem.clone();
                } else {
                    self.head.borrow_mut().next = elem.borrow().next.clone();
                    elem.borrow_mut().next = None;
                    return Some(elem.clone());
                }
            }
            if Rc::ptr_eq(&self.head, &original_head) {
                break;
            }
        }
        None
    }

    /// Pop the front element unconditionally
    pub fn pop_front_any(&mut self) -> Option<Rc<RefCell<NonOwningRingSpinElement>>> {
        self.pop_front(|_| true)
    }

    /// Clear the entire ring buffer
    pub fn clear(&mut self) {
        while self.pop_front_any().is_some() {}
    }
}
