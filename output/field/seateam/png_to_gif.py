from PIL import Image

# Take list of paths for images
image_path_list = [f'shot{i}.png' for i in range(1, 101)]

# Create a list of image objects
image_list = [Image.open(file) for file in image_path_list]

# Save the first image as a GIF file
image_list[0].save(
            'gif_sea.gif',
            save_all=True,
            append_images=image_list[1:], # append rest of the images
            duration=50, # in milliseconds
            loop=0)