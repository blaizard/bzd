use utils::arg1_to_vector;

fn part1(lines: &Vec<String>) {
    let mut counter = 0;
    let mut value = 50;
    for line in lines {
        let mut chars = line.chars();
        let dir = chars.next();
        let val: i32 = chars.as_str().parse::<i32>().unwrap() % 100;
        value = match dir.unwrap() {
            'L' => {
                if value - val >= 0 {
                    value - val
                } else {
                    100 + value - val
                }
            }
            'R' => {
                if value + val <= 99 {
                    value + val
                } else {
                    value + val - 100
                }
            }
            _ => panic!("unexpected"),
        };
        if value == 0 {
            counter += 1;
        }
    }

    println!("Part 1: {}", counter);
}

fn part2(lines: &Vec<String>) {
    let mut counter = 0;
    let mut value = 50;
    for line in lines {
        let mut chars = line.chars();
        let dir = chars.next();
        let val: i32 = chars.as_str().parse::<i32>().unwrap();

        // Full rotation.
        let mut pass_by_zero: i32 = (val as f32 / 100.0).floor() as i32;

        // Between 0 and 99.
        let val_rest = val - pass_by_zero * 100;
        let prev_value = value;
        value = match dir.unwrap() {
            'L' => value - val_rest,
            'R' => value + val_rest,
            _ => panic!("unexpected"),
        };

        if prev_value > 0 && (value <= 0 || value >= 100) {
            pass_by_zero += 1;
        }

        if value < 0 {
            value += 100;
        } else if value >= 100 {
            value -= 100;
        }

        counter += pass_by_zero;
    }

    println!("Part 2: {}", counter);
}

fn main() {
    let lines = arg1_to_vector();
    part1(&lines);
    part2(&lines);
}
