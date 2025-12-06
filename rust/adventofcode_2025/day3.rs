use std::collections::BinaryHeap;
use std::mem;
use utils::arg1_to_vector;

fn part1(banks: &Vec<String>) {
    let mut total = 0;

    for bank in banks {
        let range_size = bank.len() as i32;
        let mut c1: u32 = 0;
        let mut c2: u32 = 0;
        if range_size > 2 {
            let mut heap: BinaryHeap<(u32, i32)> = BinaryHeap::with_capacity(bank.len());
            for (index, c) in bank.chars().map(|c| c.to_digit(10).unwrap()).enumerate() {
                heap.push((c, -(index as i32)));
            }

            let index1;
            (c1, index1) = heap.pop().unwrap();
            if -index1 == range_size - 1 {
                (c2, _) = heap.pop().unwrap();
                mem::swap(&mut c1, &mut c2);
            } else {
                while let Some((val, index2)) = heap.pop() {
                    if index2 < index1 {
                        c2 = val;
                        break;
                    }
                }
            }
        } else {
            panic!("Small bank");
        }

        total += c1 * 10 + c2;
    }

    println!("Part 1: {}", total);
}

fn part2(banks: &Vec<String>) {
    let mut total: u64 = 0;

    for bank in banks {
        let range_size = bank.len() as i32;
        let mut left = 12;
        let mut count: u64 = 0;
        let mut previous_index = 1;

        let mut heap: BinaryHeap<(u32, i32)> = BinaryHeap::with_capacity(bank.len());
        for (index, c) in bank.chars().map(|c| c.to_digit(10).unwrap()).enumerate() {
            heap.push((c, -(index as i32)));

            if bank.len() - index == left {
                while let Some((val, index2)) = heap.pop() {
                    if index2 < previous_index {
                        count = count * 10 + val as u64;
                        previous_index = index2;
                        break;
                    }
                }

                left -= 1;
            }
        }

        total += count;
    }

    println!("Part 2: {}", total);
}

fn main() {
    let banks = arg1_to_vector('\n');
    part1(&banks);
    part2(&banks);
}
