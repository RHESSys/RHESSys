PARAM_DB_FILENAME = "params.sqlite"
VALID_TYPES = ("basin", "fire", "hillslope", "landuse", "soil", "stratum", "surface_energy", "zone")
VALID_FORMATS = ('csv', 'param')
OUTPUT_FORMAT_CSV = 'csv'
OUTPUT_FORMAT_PARAM = 'param'

SEARCH_TYPES = ("hierarchical", "constrained")
SEARCH_TYPE_HIERARCHICAL = SEARCH_TYPES[0]
SEARCH_TYPE_CONSTRAINED  = SEARCH_TYPES[1]

# Table field names
ADMIN_FIELD_NAMES = 'dt,user,command'
CLASS_TYPE_FIELD_NAMES = 'type_id,type_name,max_id' 
CLASS_TYPE_FIELD_NAMES_QUALIFIED = ''
for ct in CLASS_TYPE_FIELD_NAMES.split(','):
    CLASS_TYPE_FIELD_NAMES_QUALIFIED += 'ct.%s,' % ct
CLASS_TYPE_FIELD_NAMES_QUALIFIED = CLASS_TYPE_FIELD_NAMES_QUALIFIED.rstrip(',')
# These are the db column names for clas_type and class tables joined
CLASS_FIELD_NAMES = 'class_id,name,location,type_id,genus,species,default_id,parent_id' 
CLASS_FIELD_NAMES_QUALIFIED = 'c.class_id,c.name,c.location,c.type_id,c.genus,c.species,c.default_id,c.parent_id' 
TYPE_JOIN_CLASS_FIELD_NAMES = CLASS_TYPE_FIELD_NAMES + "," + CLASS_FIELD_NAMES
TYPE_JOIN_CLASS_FIELD_NAMES_QUALIFIED = CLASS_TYPE_FIELD_NAMES_QUALIFIED + "," + CLASS_FIELD_NAMES_QUALIFIED
#CLASS_FIELD_NAMES_QUALIFIED = 'c.class_id,c.name,c.location,c.type_id,c.genus,c.species,c.default_id,c.parent_id' 

#for cf in CLASS_FIELD_NAMES.split(','):
#    CLASS_FIELD_NAMES_QUALIFIED += 'c.%s,' % cf
#CLASS_FIELD_NAMES_QUALIFIED = CLASS_FIELD_NAMES_QUALIFIED.rstrip(',')

PARAM_FIELD_NAMES = 'class_id,name,value,dt,reference,comment,user'
PARAM_FIELD_NAMES_QUALIFIED = ''
for pf in PARAM_FIELD_NAMES.split(','):
    PARAM_FIELD_NAMES_QUALIFIED += 'p.%s,' % pf
PARAM_FIELD_NAMES_QUALIFIED = PARAM_FIELD_NAMES_QUALIFIED.rstrip(',')

CLASS_TYPE_IND = {'type_id':0, 'type_name':1, 'max_id':2}
# This dict must match 'CLASS_FILE_NAMES' and 'CLASS_FIELD_NAMES_QUALIFIED'
# ct.type_id,ct.type_name,ct.max_id,c.class_id,c.name,c.location,c.type_id,c.genus,c.species,c.default_id,c.parent_id
TYPE_JOIN_CLASS_IND = {'type_id':0, 'type_name':1, 'max_id':2, 'class_id':3, 'name':4, 'location':5, 'type_id':6, 'genus':7, 'species':8, 'default_id':9, 'parent_id':10}
CLASS_IND = {'class_id':0, 'name':1, 'location':2, 'type_id':3, 'genus':4, 'species':5, 'default_id':6, 'parent_id':7}
PARAM_IND = {'class_id':0, 'name':1, 'value': 2, 'dt': 3, 'reference': 4, 'comment': 5, 'user': 6}

