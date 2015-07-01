/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2015 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "rocrail/impl/xmlscript_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/text.h"
#include "rocrail/public/var.h"
#include "rocrail/public/loc.h"

#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Variable.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/strtok.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OXmlScript ----- */


/**  */

static Boolean __isCondition(const char* conditionRes) {
  Boolean ok = True;
  /* ToDo: Check the condition.
   * "#var2%oid% < &time" */
  const char* var  = NULL;
  const char* comparator = NULL;
  const char* value = NULL;
  iOStrTok tok = StrTokOp.inst(conditionRes, ' ');

  if( StrTokOp.hasMoreTokens(tok) ) {
    var = StrTokOp.nextToken(tok);
  }
  if( StrTokOp.hasMoreTokens(tok) ) {
    comparator = StrTokOp.nextToken(tok);
  }
  if( StrTokOp.hasMoreTokens(tok) ) {
    value = StrTokOp.nextToken(tok);
  }

  if( var != NULL && comparator != NULL && value != NULL ) {
    int varValue   = VarOp.getValue(var, NULL);
    int valueValue = VarOp.getValue(value, NULL);
    if( comparator[0] == '<' ) {
      if( varValue >= valueValue )
        ok = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "condition [%s] is %s: %d < %d", conditionRes, ok?"true":"false", varValue, valueValue );
    }
    else if( comparator[0] == '>' ) {
      if( varValue <= valueValue )
        ok = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "condition [%s] is %s: %d > %d", conditionRes, ok?"true":"false", varValue, valueValue );
    }
    else if( comparator[0] == '=' ) {
      if( varValue != valueValue )
        ok = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "condition [%s] is %s: %d == %d", conditionRes, ok?"true":"false", varValue, valueValue );
    }
  }

  StrTokOp.base.del(tok);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "condition [%s] is %s", conditionRes, ok?"true":"false" );
  return ok;
}

static void __executeCmd(iONode cmd) {
  iOModel model = AppOp.getModel();
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "execute [%s] id[%s]", NodeOp.getName(cmd), wItem.getid(cmd) );

  /* loco */
  if( StrOp.equals( wFunCmd.name(), NodeOp.getName(cmd)) || StrOp.equals( wLoc.name(), NodeOp.getName(cmd)) ) {
    iOLoc lc = ModelOp.getLoc(model, wItem.getid(cmd), NULL, False);
    if( lc != NULL )
      LocOp.cmd(lc, (iONode)NodeOp.base.clone(cmd));
  }

  /* var */
  else if( StrOp.equals( wVariable.name(), NodeOp.getName(cmd)) ) {
    iOMap map = MapOp.inst();
    const char* oid = wItem.getid(cmd);
    MapOp.put(map, "oid", (obj)oid);
    char* varRes = TextOp.replaceAllSubstitutions(oid, map);
    MapOp.base.del(map);

    iONode var = ModelOp.getVariable(model, varRes);
    if( var != NULL ) {
      wVariable.setvalue(var, VarOp.getValue(wVariable.getvalstr(cmd), NULL));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "var [%s] = %d", varRes, wVariable.getvalue(var) );
    }
    StrOp.free( varRes );
  }
}


static void __doIf(iONode nodeScript, iOMap map) {
  const char* condition = NodeOp.getStr(nodeScript, "condition", NULL);
  if( condition == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "skip: condition is missing in the if statement" );
    return;
  }
  char* conditionRes = TextOp.replaceAllSubstitutions(condition, map);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "if condition [%s]", conditionRes );

  if( __isCondition(conditionRes) ) {
    iONode thenNode = NodeOp.findNode(nodeScript, "then");
    if( thenNode != NULL ) {
      int cmds = NodeOp.getChildCnt(thenNode);
      int n = 0;
      for( n = 0; n < cmds; n++ ) {
        iONode cmd = NodeOp.getChild(thenNode, n);
        char* id = TextOp.replaceAllSubstitutions(wItem.getid(cmd), map);
        wItem.setid(cmd, id);
        __executeCmd(cmd);
        StrOp.free(id);
      }
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "then is missing in the if statement" );
  }
  else {
    iONode elseNode = NodeOp.findNode(nodeScript, "else");
    if( elseNode != NULL ) {
      int cmds = NodeOp.getChildCnt(elseNode);
      int n = 0;
      for( n = 0; n < cmds; n++ ) {
        iONode cmd = NodeOp.getChild(elseNode, n);
        char* id = TextOp.replaceAllSubstitutions(wItem.getid(cmd), map);
        wItem.setid(cmd, id);
        __executeCmd(cmd);
        StrOp.free(id);
      }
    }
  }

  StrOp.free(conditionRes);
}


static void __doForEach(iONode nodeScript, iOMap map) {
  iOModel model = AppOp.getModel();
  iONode plan = ModelOp.getModel(model);
  iONode table = NodeOp.findNode(plan, NodeOp.getStr(nodeScript, "table", ""));
  const char* condition = NodeOp.getStr(nodeScript, "condition", NULL);
  if( table != NULL ) {
    int childs = NodeOp.getChildCnt(table);
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "for each in table [%s] condition [%s]", NodeOp.getName(table), condition!=NULL?condition:"-" );
    for( i = 0; i < childs; i++ ) {
      iONode child = NodeOp.getChild( table, i);
      const char* oid = wItem.getid(child);
      char* conditionRes = NULL;
      if( condition != NULL ) {
        MapOp.put(map, "oid", (obj)oid);
        conditionRes = TextOp.replaceAllSubstitutions(condition, map);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "for each in table [%s] condition [%s]", NodeOp.getName(table), conditionRes );
      }

      if( conditionRes == NULL || __isCondition(conditionRes) ) {
        int cmds = NodeOp.getChildCnt(nodeScript);
        int n = 0;
        for( n = 0; n < cmds; n++ ) {
          iONode cmd = NodeOp.getChild(nodeScript, n);
          wItem.setid(cmd, oid);
          __executeCmd(cmd);
        }
      }

      StrOp.free(conditionRes);
    }


  }
}


/*
<foreach table="lclist" condition="#var2%oid% < &time">
  <fn f3="true"/>
  <var id="var2%oid%" val="0"/>
  <lc cmd="go"/>
</foreach>
 */
static void _run(const char* script, iOMap map) {
  iODoc  doc        = DocOp.parse(script);
  iONode nodeScript = NULL;
  if( doc != NULL && DocOp.getRootNode(doc) != NULL) {
    nodeScript = DocOp.getRootNode(doc);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "script: [%s]", NodeOp.getName(nodeScript) );
    if( StrOp.equals( "foreach", NodeOp.getName(nodeScript) ) ) {
      __doForEach(nodeScript, map);
    }
    else if( StrOp.equals( "if", NodeOp.getName(nodeScript) ) ) {
      __doIf(nodeScript, map);
    }

    NodeOp.base.del(nodeScript);
  }

  if( doc != NULL)
    DocOp.base.del(doc);

}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/xmlscript.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
