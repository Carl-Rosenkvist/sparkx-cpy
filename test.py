import numpy as np
from binary_reader import FieldType, Field, get_particle_objects
from sparkx.Particle import Particle

# Define fields correctly
fields = [
    Field("mass", FieldType.Double),
    Field("E", FieldType.Double),
    Field("pz", FieldType.Double),
    Field("pdg", FieldType.Int32),  # Corrected: Use Int32
    Field("ncoll", FieldType.Int32),
]


extended_fields = [
    Field("t", FieldType.Double),
    Field("x", FieldType.Double),
    Field("y", FieldType.Double),
    Field("z", FieldType.Double),
    
    Field("mass", FieldType.Double),
    
    Field("E", FieldType.Double),
    Field("px", FieldType.Double),
    Field("py", FieldType.Double),
    Field("pz", FieldType.Double),
   
    Field("pdg", FieldType.Int32), 
    Field("ID", FieldType.Int32),  
    Field("charge", FieldType.Int32),
    Field("ncoll", FieldType.Int32),
    
    Field("form_time", FieldType.Double),
    Field("xsecfac", FieldType.Double),

    Field("proc_id_origin", FieldType.Int32),  
    Field("proc_type_origin", FieldType.Int32),  


    Field("t_last_coll", FieldType.Double),

    Field("pdg_mother1", FieldType.Int32), 
    Field("pdg_mother2", FieldType.Int32), 
    Field("baryon_number", FieldType.Int32), 
    Field("strangeness", FieldType.Int32), 




]


# Read particle objects from binary file

#particles = get_particle_objects("/Users/carl/Phd/systemSize/test/out-1/particles_binary.bin", fields)
particles = get_particle_objects("/Users/carl/Phd/sparxk_bench/out_nevents_800/particles_binary.bin", extended_fields)
# Extract PDG values
#pdgs = [particle.pdg for particle in particles]

# Print unique PDG values
#print(np.unique(pdgs))
