use utils::arg1_to_vector;

fn part1(bool_grid: &Vec<Vec<bool>>) {
    let max_x = (bool_grid[0].len() - 1) as isize;
    let max_y = (bool_grid.len() - 1) as isize;
    let count_rool = |x: isize, y: isize| {
        if x < 0 {
            return 0;
        }
        if y < 0 {
            return 0;
        }
        if x > max_x {
            return 0;
        }
        if y > max_y {
            return 0;
        }
        if bool_grid[y as usize][x as usize] {
            1
        } else {
            0
        }
    };
    let mut total = 0;

    for y in 0..=max_y {
        for x in 0..=max_x {
            if bool_grid[y as usize][x as usize] {
                let mut count = 0;
                count += count_rool(x - 1, y - 1);
                count += count_rool(x - 1, y);
                count += count_rool(x - 1, y + 1);
                count += count_rool(x, y - 1);
                count += count_rool(x, y + 1);
                count += count_rool(x + 1, y - 1);
                count += count_rool(x + 1, y);
                count += count_rool(x + 1, y + 1);
                if count < 4 {
                    total += 1;
                }
            }
        }
    }

    println!("Part 1: {}", total);
}

fn part2(bool_grid: &mut Vec<Vec<bool>>) {
    let max_x = (bool_grid[0].len() - 1) as isize;
    let max_y = (bool_grid.len() - 1) as isize;
    let count_rool = |grid: &Vec<Vec<bool>>, x: isize, y: isize| {
        if x < 0 {
            return 0;
        }
        if y < 0 {
            return 0;
        }
        if x > max_x {
            return 0;
        }
        if y > max_y {
            return 0;
        }
        if grid[y as usize][x as usize] {
            1
        } else {
            0
        }
    };

    let mut sum = 0;
    loop {
        let mut total = 0;
        for y in 0..=max_y {
            for x in 0..=max_x {
                if bool_grid[y as usize][x as usize] {
                    let mut count = 0;
                    count += count_rool(&bool_grid, x - 1, y - 1);
                    count += count_rool(&bool_grid, x - 1, y);
                    count += count_rool(&bool_grid, x - 1, y + 1);
                    count += count_rool(&bool_grid, x, y - 1);
                    count += count_rool(&bool_grid, x, y + 1);
                    count += count_rool(&bool_grid, x + 1, y - 1);
                    count += count_rool(&bool_grid, x + 1, y);
                    count += count_rool(&bool_grid, x + 1, y + 1);
                    if count < 4 {
                        total += 1;
                        bool_grid[y as usize][x as usize] = false;
                    }
                }
            }
        }
        if total == 0 {
            break;
        }
        sum += total;
    }

    println!("Part 2: {}", sum);
}

fn main() {
    let lines = arg1_to_vector('\n');
    let mut bool_grid: Vec<Vec<bool>> = lines
        .iter()
        .map(|line| line.chars().map(|c| c == '@').collect())
        .collect();
    part1(&bool_grid);
    part2(&mut bool_grid);
}
