/* Copyright (c) 2014, 2017, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#include "dd/impl/tables/column_type_elements.h"

#include <new>

#include "dd/impl/raw/object_keys.h"  // Parent_id_range_key
#include "dd/impl/types/object_table_definition_impl.h"
#include "my_dbug.h"
#include "sql_const.h"                // MAX_INTERVAL_VALUE_LENGTH

namespace dd {
namespace tables {

const Column_type_elements &Column_type_elements::instance()
{
  static Column_type_elements *s_instance= new Column_type_elements();
  return *s_instance;
}

///////////////////////////////////////////////////////////////////////////

Column_type_elements::Column_type_elements()
{
  m_target_def.table_name(table_name());
  m_target_def.dd_version(1);

  m_target_def.add_field(FIELD_COLUMN_ID,
                         "FIELD_COLUMN_ID",
                         "column_id BIGINT UNSIGNED NOT NULL");
  m_target_def.add_field(FIELD_INDEX,
                         "FIELD_INDEX",
                         "element_index INT UNSIGNED NOT NULL");
  // Fail if the max length of enum/set elements is increased.
  // If it's changed, the corresponding column length must be
  // increased, but this must be treated as a DD table upgrade
  // requiring special care.
  DBUG_ASSERT(MAX_INTERVAL_VALUE_LENGTH <= 255);

  // Leave room for four bytes per character, which is used
  // by e.g. utf8mb4, i.e. 255 * 4 = 1020 bytes.
  m_target_def.add_field(FIELD_NAME,
                         "FIELD_NAME",
                         "name VARBINARY(1020) NOT NULL");

  m_target_def.add_index("PRIMARY KEY(column_id, element_index)");
  // We may have multiple similar element names. Do we plan to deprecate it?
  // m_target_def.add_index("UNIQUE KEY(column_id, name)");

  m_target_def.add_foreign_key("FOREIGN KEY (column_id) REFERENCES "
                               "columns(id)");
}

///////////////////////////////////////////////////////////////////////////

Object_key *Column_type_elements::create_key_by_column_id(
  Object_id column_id)
{
  return new (std::nothrow) Parent_id_range_key(0, FIELD_COLUMN_ID, column_id);
}

///////////////////////////////////////////////////////////////////////////

Object_key *Column_type_elements::create_primary_key(
  Object_id column_id, int index)
{
  const int INDEX_NO= 0;

  return new (std::nothrow) Composite_pk(INDEX_NO,
                                         FIELD_COLUMN_ID, column_id,
                                         FIELD_INDEX, index);
}

///////////////////////////////////////////////////////////////////////////

}
}
