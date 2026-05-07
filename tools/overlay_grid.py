from pathlib import Path
import argparse

from PIL import Image, ImageDraw


def parse_color(value: str) -> tuple[int, int, int, int]:
    parts = [int(part) for part in value.split(",")]
    if len(parts) == 3:
        return parts[0], parts[1], parts[2], 255
    if len(parts) == 4:
        return parts[0], parts[1], parts[2], parts[3]
    raise ValueError("Color must be R,G,B or R,G,B,A")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Clone an image and draw a grid overlay on top of it."
    )
    parser.add_argument(
        "input",
        nargs="?",
        default="assets/klad1/sprites.png",
        help="Input image path",
    )
    parser.add_argument(
        "output",
        nargs="?",
        default="assets/klad1/sprites_grid_22.png",
        help="Output image path",
    )
    parser.add_argument("--cell-width", type=int, default=22, help="Grid cell width")
    parser.add_argument("--cell-height", type=int, default=22, help="Grid cell height")
    parser.add_argument(
        "--color",
        default="255,0,0,180",
        help="Grid color as R,G,B or R,G,B,A",
    )
    parser.add_argument(
        "--axis-color",
        default="0,255,255,255",
        help="Axis color as R,G,B or R,G,B,A",
    )
    args = parser.parse_args()

    input_path = Path(args.input)
    output_path = Path(args.output)

    image = Image.open(input_path).convert("RGBA")
    overlay = Image.new("RGBA", image.size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(overlay)

    grid_color = parse_color(args.color)
    axis_color = parse_color(args.axis_color)

    width, height = image.size

    for x in range(0, width + 1, args.cell_width):
        color = axis_color if x == 0 else grid_color
        draw.line([(x, 0), (x, height)], fill=color, width=1)

    for y in range(0, height + 1, args.cell_height):
        color = axis_color if y == 0 else grid_color
        draw.line([(0, y), (width, y)], fill=color, width=1)

    result = Image.alpha_composite(image, overlay)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    result.save(output_path)

    print(f"Saved grid overlay to {output_path}")
    print(f"Image size: {width}x{height}")
    print(f"Cell size: {args.cell_width}x{args.cell_height}")


if __name__ == "__main__":
    main()
