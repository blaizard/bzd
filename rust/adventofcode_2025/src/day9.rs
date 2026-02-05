use utils::arg1_to_vector;

fn part1(tiles: &Vec<(i64, i64)>) {
    let mut total = 0;

    let _is_empty = |x1: i64, y1: i64, x2: i64, y2: i64, tiles: &Vec<(i64, i64)>| -> bool {
        for (x, y) in tiles {
            if x > &x1 && x < &x2 {
                return false;
            }
            if y > &y1 && y < &y2 {
                return false;
            }
        }
        true
    };

    // Check rectangle that are empty
    for (x1, y1) in tiles {
        for (x2, y2) in tiles {
            // Area of 0, ignore.
            if x1 == x2 || y1 == y2 {
                continue;
            }
            let (xo1, xo2) = if x1 < x2 { (x1, x2) } else { (x2, x1) };
            let (yo1, yo2) = if y1 < y2 { (y1, y2) } else { (y2, y1) };
            //if is_empty(*xo1, *yo1, *xo2, *yo2, tiles) {
            let area = (xo2 - xo1 + 1) * (yo2 - yo1 + 1);
            //println!("Area {} ({},{}) ({},{})", area, x1, y1, x2, y2);
            total = total.max(area);
            //}
        }
    }

    println!("Part 1: {}", total);
}

fn part2(_tiles: &[(i64, i64)]) {
    let total = 0;
    println!("Part 2: {}", total);
}

fn main() {
    let lines = arg1_to_vector('\n', true);
    let tiles = lines
        .iter()
        .map(|x| {
            let (x, y) = x.split_once(',').unwrap();
            (x.parse::<i64>().unwrap(), y.parse::<i64>().unwrap())
        })
        .collect();
    part1(&tiles);
    part2(&tiles);
}
