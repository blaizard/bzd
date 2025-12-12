use utils::arg1_to_vector;

fn part1(ranges: &Vec<String>) {
    let mut invalid = 0;
    for range in ranges {
        let (left_str, right_str) = range.split_once("-").unwrap();
        let (left, right) = (
            left_str.parse::<u64>().unwrap(),
            right_str.parse::<u64>().unwrap(),
        );

        for id in left..=right {
            let id_str = id.to_string();
            if id_str.len() % 2 == 0 {
                let (left_id_str, right_id_str) = id_str.split_at(id_str.len() / 2);
                if left_id_str == right_id_str {
                    invalid += id;
                }
            }
        }
    }

    println!("Part 1: {}", invalid);
}

fn part2(ranges: &Vec<String>) {
    let mut invalid = 0;
    for range in ranges {
        let (left_str, right_str) = range.split_once("-").unwrap();
        let (left, right) = (
            left_str.parse::<u64>().unwrap(),
            right_str.parse::<u64>().unwrap(),
        );

        for id in left..=right {
            let id_str = id.to_string();
            for index in 1..=(id_str.len() / 2) {
                if id_str.len() % index == 0 {
                    let mut same = true;
                    let (test1, mut rest) = id_str.split_at(index);
                    while rest.len() > 0 {
                        let test2;
                        (test2, rest) = rest.split_at(index);
                        if test1 != test2 {
                            same = false;
                            break;
                        }
                    }
                    if same == true {
                        invalid += id;
                        break;
                    }
                }
            }
        }
    }

    println!("Part 2: {}", invalid);
}

fn main() {
    let ranges = arg1_to_vector(',', true);
    part1(&ranges);
    part2(&ranges);
}
