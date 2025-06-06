import jellyfish
import clang.cindex
from clang.cindex import CursorKind, TokenKind, Config

class CppFunctionParser:
    def __init__(self):
        Config.set_library_path('/usr/lib/llvm-14/lib')
        self.system_includes = [
            '/gcc',
            '/lib64',
            '/usr/include',
            '/usr/include/c++',
            '/usr/lib/clang',
            'Windows Kits'
        ]
        self.hashed_functions = {}

    def is_system_header(self, file_path):
        if not file_path:
            return False
        return any(sys_path in file_path for sys_path in self.system_includes)

    def get_token_info(self, token, cursor):
        return {
            'spelling': token.spelling,
            'token_kind': str(token.kind).split('.')[1],
            'cursor_kind': str(cursor.kind).split('.')[1] if cursor.kind else None,
            'type': cursor.type.spelling if hasattr(cursor, 'type') else None
        }

    def tokenize_function_body(self, cursor):
        tokens = []
        source_range = cursor.extent
        tu = cursor.translation_unit
        token_set = tu.get_tokens(extent=source_range)
        
        found_body_start = False
        
        for token in token_set:
            if not found_body_start:
                if token.kind == TokenKind.PUNCTUATION and token.spelling == '{':
                    found_body_start = True
                continue
            
            if token.kind == TokenKind.PUNCTUATION and token.spelling == '}':
                break
                
            token_cursor = clang.cindex.Cursor.from_location(tu, token.location)
            tokens.append(self.get_token_info(token, token_cursor))
        
        return tokens

    def parse_functions(self, file_path):
        index = clang.cindex.Index.create()
        tu = index.parse(file_path, args=['-std=c++17'])
        
        functions = []
        
        for cursor in tu.cursor.walk_preorder():
            try:
                if (cursor.kind in [CursorKind.FUNCTION_DECL, CursorKind.CXX_METHOD] 
                    and cursor.is_definition()):
                    
                    if self.is_system_header(cursor.location.file.name):
                        continue
                    
                    func_data = {
                        'name': cursor.spelling,
                        'return_type': cursor.type.get_result().spelling,
                        'parameters': self.get_parameters(cursor),
                        'tokens': self.tokenize_function_body(cursor)
                    }
                    functions.append(func_data)
            except Exception as e:
                print(f"Error processing cursor: {e}")
        
        return functions

    def get_parameters(self, cursor):
        parameters = []
        for arg in cursor.get_arguments():
            parameters.append(arg.type.spelling)
        return parameters

    def hash_functions(self, functions):
        for func in functions:
            func_hash = ''
            func_id = ''
            for token in func['tokens']:
                func_hash += token['token_kind'][0]

 
            func_id = set(func['parameters']) if len(func['parameters']) > 0 else set()
            func_id.add(func['return_type'])
            self.hashed_functions[func['name']] =  [func_hash, func_id] 
                
    def print_function_info(self, file_path):
        functions = self.parse_functions(file_path)
        self.hash_functions(functions)
        

def compare_hashes(cfp1, cfp2):
    comparison_results = {} 
    common_names = set(cfp1.hashed_functions.keys()) & set(cfp2.hashed_functions.keys()) 
    for name in common_names:
        hash1, id1 = cfp1.hashed_functions[name]
        hash2, id2 = cfp2.hashed_functions[name]
        
        id_similar = id1 == id2
        
        if len(hash1) == 0 or len(hash2) == 0:
            similarity = 0.0
        else:
            distance = jellyfish.levenshtein_distance(hash1, hash2)
            max_len = max(len(hash1), len(hash2))
            similarity = 1 - distance / max_len
        
        comparison_results[name] = {
            'id_match': id_similar,
            'similarity': similarity,
            'hash1': hash1,
            'hash2': hash2,
            'id1': sorted(id1),
            'id2': sorted(id2)
        }    
    return comparison_results

if __name__ == "__main__":
    parser = CppFunctionParser()
    parser2 = CppFunctionParser()
    sample_file = "../test_samples/Vector (1).cpp"
    sample_file2 = "../test_samples/Vector (2).cpp"
    
    parser.print_function_info(sample_file)
    parser2.print_function_info(sample_file2) 
    results = compare_hashes(parser, parser2)
    

    print("Результаты сравнения файлов ", sample_file, ' и ', sample_file2)
    for func_name, data in results.items():
        print('Функции', func_name, 'похожи на ', round(data['similarity'] * 100), '% ')
        print(data , '\n')

