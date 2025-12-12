use utils::arg1_to_vector;

fn part1(numbers: &Vec<Vec<i64>>, operations: &Vec<char>) {
    let mut total = 0;

    for (index, operation) in operations.iter().enumerate() {
        let mut result = numbers[0][index];
        for number_line in numbers[1..].iter() {
            if *operation == '*' {
                result *= number_line[index];
            } else if *operation == '+' {
                result += number_line[index];
            } else {
                panic!("Unsupported operation {}", *operation);
            }
        }
        total += result;
    }

    println!("Part 1: {}", total);
}

fn part2(numbers: &Vec<Vec<i64>>, operations: &Vec<char>) {
    let mut total = 0;

    for (index, operation) in operations.iter().enumerate() {
        let mut result = numbers[index][0];
        for number in numbers[index][1..].iter() {
            if *operation == '*' {
                result *= number;
            } else if *operation == '+' {
                result += number;
            } else {
                panic!("Unsupported operation {}", *operation);
            }
        }
        total += result;
    }

    println!("Part 2: {}", total);
}

fn main() {
    let mut lines = arg1_to_vector('\n', false);

    let operations: Vec<char> = lines
        .pop()
        .unwrap()
        .split(" ")
        .filter_map(|x| x.trim().chars().nth(0))
        .collect();
    let numbers: Vec<Vec<i64>> = lines
        .iter()
        .map(|x| {
            x.split(" ")
                .filter_map(|y| y.trim().parse::<i64>().ok())
                .collect()
        })
        .collect();

    let expected_count = operations.len();
    for number in &numbers {
        assert!(number.len() == expected_count);
    }

    let mut lines_flipped: Vec<String> = vec![];
    lines_flipped.resize(lines[0].len(), "".to_string());
    for line in &lines {
        for (index, c) in line.chars().enumerate() {
            lines_flipped[index].push(c);
        }
    }
    let mut numbers2: Vec<Vec<i64>> = vec![];
    let mut index = 0;
    numbers2.push(Vec::new());
    for line in &lines_flipped {
        match line.trim().parse::<i64>() {
            Ok(v) => numbers2[index].push(v),
            Err(_e) => {
                index += 1;
                numbers2.push(Vec::new());
            }
        }
    }

    part1(&numbers, &operations);
    part2(&numbers2, &operations);
}
