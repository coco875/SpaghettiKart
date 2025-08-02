import os
import yaml

def enumerate_yaml_files(directory):
    """Recursively enumerate all YAML files in the given directory."""
    for root, dirs, files in os.walk(directory):
        for filename in files:
            if filename.endswith(".yaml") or filename.endswith(".yml"):
                yield os.path.join(root, filename)

def get_element(directory):
    elements: set = set()
    for yaml_file in enumerate_yaml_files(directory):
        with open(yaml_file, 'r') as file:
            try:
                data = yaml.safe_load(file)
                assert isinstance(data, dict), "YAML file must contain a dictionary"
                for key, value in data.items():
                    if key == ":config":
                        continue
                    if key in elements:
                        print(f"Duplicate key '{key}' found in {yaml_file}")
                    elements.add(key)
            except yaml.YAMLError as e:
                print(f"Error parsing {yaml_file}: {e}")
    return elements

el1 = get_element("yamls_old")
el2 = get_element("yamls")

for el in el1:
    if el not in el2:
        print(f"Element '{el}' found in 'yamls_old' but not in 'yamls'")