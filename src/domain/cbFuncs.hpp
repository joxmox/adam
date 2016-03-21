/*
 * cbFuncs.hpp
 *
 *  Created on: Mar 17, 2016
 *      Author: joxmox
 */

#ifndef SRC_DOMAIN_CBFUNCS_HPP_
#define SRC_DOMAIN_CBFUNCS_HPP_

#include <string>
#include <vector>

#include "Editor.hpp"

using namespace std;

void cbIllegalChar(Editor* ed, const vector<string>& params) {ed->illegalChar();}
void cbNormalChar(Editor* ed, const vector<string>& params) {ed->normalChar();}
void cbExit(Editor* ed, const vector<string>& params) {ed->exit();}
void cbQuit(Editor* ed, const vector<string>& params) {ed->quit();}
void cbDebug(Editor* ed, const vector<string>& params) {ed->debug();}
void cbMoveUp(Editor* ed, const vector<string>& params) {ed->moveUp();}
void cbMoveDown(Editor* ed, const vector<string>& params) {ed->moveDown();}
void cbMoveLeft(Editor* ed, const vector<string>& params) {ed->moveLeft();}
void cbMoveRight(Editor* ed, const vector<string>& params) {ed->moveRight();}
void cbInsertBreak(Editor* ed, const vector<string>& params) {ed->insertBreak();}
void cbBackSpace(Editor* ed, const vector<string>& params) {ed->backSpace();}
void cbGotoSol(Editor* ed, const vector<string>& params) {ed->gotoSol();}
void cbGotoEol(Editor* ed, const vector<string>& params) {ed->gotoEol();}
void cbStartLearn(Editor* ed, const vector<string>& params) {ed->startLearn();}
void cbRemember(Editor* ed, const vector<string>& params) {ed->remember();}
void cbDoLearned(Editor* ed, const vector<string>& params) {ed->doLearned();}
void cbPageUp(Editor* ed, const vector<string>& params) {ed->pageUp();}
void cbPageDown(Editor* ed, const vector<string>& params) {ed->pageDown();}
void cbKillLine(Editor* ed, const vector<string>& params) {ed->killLine();}
void cbInsert(Editor* ed, const vector<string>& params) {ed->insert();}
void cbDo(Editor* ed, const vector<string>& params) {ed->doCommand();};
void cbWriteFile(Editor* ed, const vector<string>& params) {ed->writeFile(params);}
void cbGotoLine(Editor* ed, const vector<string>& params) {ed->gotoLine(params[0]);}
void cbGotoMark(Editor* ed, const vector<string>& params) {ed->gotoMark(params[0]);}
void cbSetMark(Editor* ed, const vector<string>& params) {ed->setMark(params[0]);}
void cbWhere(Editor* ed, const vector<string>& params) {ed->where();}
void cbGotoTop(Editor* ed, const vector<string>& params) {ed->gotoTop();};
void cbGotoBot(Editor* ed, const vector<string>& params) {ed->gotoBot();};

void cbParseError(Editor* ed, int errCode) {ed->parseError(errCode);}


#endif /* SRC_DOMAIN_CBFUNCS_HPP_ */
