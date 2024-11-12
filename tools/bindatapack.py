import os
import sys

def generate_resource_data(resource_dir, output_file):
    with open(output_file, 'w') as f:
        f.write('// Auto-generated resource data\n\n')
        
        for root, dirs, files in os.walk(resource_dir):
            for file in files:
                if file.endswith(('.png', '.svg', '.ttf')):
                    filepath = os.path.join(root, file)
                    with open(filepath, 'rb') as res:
                        data = res.read()
                        var_name = os.path.splitext(file)[0].replace(' ', '_').replace('-', '_')
                        
                        f.write(f'static const unsigned char {var_name}[] = {{\n')
                        for i, byte in enumerate(data):
                            if i % 16 == 0:
                                f.write('    ')
                            f.write(f'0x{byte:02x},')
                            if i % 16 == 15:
                                f.write('\n')
                        f.write('\n};\n\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('Usage: python generate_resources.py <resource_dir> <output_file>')
        sys.exit(1)
        
    generate_resource_data(sys.argv[1], sys.argv[2])