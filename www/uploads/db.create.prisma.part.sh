#!/bin/bash

if [ $# -eq 0 ]; then
    read -p "Enter new db part name:" INPUT
    PROJECTS=($INPUT)
else
    PROJECTS=("$@")
fi

for PROJECT in "${PROJECTS[@]}"; do
	LOWER=$(echo "$PROJECT" | tr '[:upper:]' '[:lower:]')
	CAPITALIZED="$(echo ${LOWER:0:1} | tr '[:lower:]' '[:upper:]')${LOWER:1}"

    echo "Created prisma part $LOWER..."
    mkdir -p "$LOWER"

    FILE1="${LOWER}/prisma.${LOWER}.repository.ts"

	

cat <<EOF > "$FILE1"
import { Injectable } from '@nestjs/common';
import { PrismaService } from '../../prisma/prisma.service';
import { I${CAPITALIZED}Repository } from './${LOWER}.repository.interface';
import { ${CAPITALIZED} } from '@prisma/client';
import { Create${CAPITALIZED}Dto, Update${CAPITALIZED}Dto } from 'src/interfaces/${LOWER}.interface';

@Injectable()
export class Prisma${CAPITALIZED}Repository implements I${CAPITALIZED}Repository {
  constructor(private prisma: PrismaService) {}

  async getAll(): Promise<${CAPITALIZED}[]> {
    return this.prisma.${LOWER}.findMany();
  }

  async getById(id: string): Promise<${CAPITALIZED} | undefined> {
    const ${LOWER} = await this.prisma.${LOWER}.findUnique({ where: { id } });
    if (!${LOWER}) return undefined;
    return ${LOWER};
  }

  async create(dto: Create${CAPITALIZED}Dto): Promise<${CAPITALIZED}> {
    return this.prisma.${LOWER}.create({ data: dto });
  }

  async update(id: string, dto: Update${CAPITALIZED}Dto): Promise<${CAPITALIZED} | undefined> {
    try {
      return await this.prisma.${LOWER}.update({ where: { id }, data });
    } catch (e) {
      return undefined;
    }
  }

  async delete(id: string): Promise<boolean> {
    try {
      await this.prisma.${LOWER}.delete({ where: { id } });
      return true;
    } catch {
      return false;
    }
  }
}
EOF

done

echo "Created All requested DB prisma parts"

 