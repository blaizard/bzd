use utils::arg1_to_vector;

fn part1(lines: &[String]) {
    let mut total = 0;
    let mut lines_of_chars: Vec<Vec<char>> = lines.iter().map(|x| x.chars().collect()).collect();

    for index_row in 1..lines_of_chars.len() {
        let (upper, lower) = lines_of_chars.split_at_mut(index_row);
        let previous = &upper[index_row - 1];
        let current_row = &mut lower[0];
        for index_col in 0..current_row.len() {
            let c_previous = previous[index_col];
            if c_previous == 'S' {
                current_row[index_col] = '|';
            } else if c_previous == '|' {
                if current_row[index_col] == '.' {
                    current_row[index_col] = '|';
                } else if current_row[index_col] == '^' {
                    current_row[index_col - 1] = '|';
                    current_row[index_col + 1] = '|';
                    total += 1;
                }
            }
        }
    }

    println!("Part 1: {}", total);
}

fn part2(lines: &[String]) {
    let mut lines_of_chars: Vec<Vec<char>> = lines.iter().map(|x| x.chars().collect()).collect();
    let mut number_of_lines: Vec<u64> = vec![0; lines_of_chars[0].len()];

    for index_row in 1..lines_of_chars.len() {
        let (upper, lower) = lines_of_chars.split_at_mut(index_row);
        let previous = &upper[index_row - 1];
        let current_row = &mut lower[0];
        for index_col in 0..current_row.len() {
            let c_previous = previous[index_col];
            if c_previous == 'S' {
                current_row[index_col] = '|';
                number_of_lines[index_col] = 1;
            } else if c_previous == '|' {
                if current_row[index_col] == '.' {
                    current_row[index_col] = '|';
                    // number_of_lines[index_col] stay the same
                } else if current_row[index_col] == '^' {
                    current_row[index_col - 1] = '|';
                    current_row[index_col + 1] = '|';
                    number_of_lines[index_col - 1] += number_of_lines[index_col];
                    number_of_lines[index_col + 1] += number_of_lines[index_col];
                    number_of_lines[index_col] = 0;
                }
            }
        }
    }

    println!("Part 2: {}", number_of_lines.iter().sum::<u64>());
}

fn main() {
    let lines = arg1_to_vector('\n', true);
    part1(&lines);
    part2(&lines);
}
