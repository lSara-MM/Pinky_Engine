# Pinky Engine
C++ Game Engine

[![PinkyEngine - Download](https://img.shields.io/static/v1?label=PinkyEngine&message=Download&color=blue&logo=github)](https://github.com/lSara-MM/Pinky_Engine/releases/download/v.0.2/Pinky_Engine_v.0.2.zip "Download Engine") <t> [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=for-the-badge&logo=github&logoColor=white)](https://github.com/lSara-MM/Pinky_Engine)

## About Pinky Engine

Pinky Engine is a Game Engine made by Andreu Nosàs Soler and Sara Qiao Martínez Mauri, as part of a college assignment from CITM (UPC) University, Terrassa, for the 3D engines subject during 3rd year of Game Design and Game Development degree.

![Pinky](https://github.com/lSara-MM/Pinky_Engine/assets/93879867/ed391b11-5df6-4e11-956c-bab93c571d51)

![image](https://github.com/lSara-MM/Pinky_Engine/assets/93879867/3dbef10b-5e6b-4caf-b42c-89cf877f54ad)

## Team Members

| Sara Qiao Martínez Mauri | Andreu Nosàs Soler |
| ----------- | ----------- |
| ![Fallout series](https://external-preview.redd.it/f8P_i_fMd-mPtdVyRiFMKOy88ciPVew4CGU9YmJ_W7s.jpg?width=640&crop=smart&auto=webp&s=51194a7f6424204858a10b688b1b05565caa826f) | ![Mass Effect 2007](http://cdn3.whatculture.com/wp-content/uploads/2015/11/3ehM1gNO.gif)|
| [![lSara-MM - ✧](https://img.shields.io/static/v1?label=lSara-MM&message=✧&color=blue&logo=github)](https://github.com/lSara-MM "Go to GitHub repo") <br> - GameObjects <br>  - Custom File Format <br> - Resource Manager <br> - Scene Serialization <br>  - Engine Windows <br>  - Component Mesh <br>  - Component Material <br>  - UI Components | [![AndyCubico - ✻](https://img.shields.io/static/v1?label=AndyCubico&message=✻&color=blue&logo=github)](https://github.com/AndyCubico "Go to GitHub repo") <br> - Frustum & Frustum Culling <br> - Mouse Picking <br> - Camera <br> - Component Transform <br>  - UI Components <br>  - Viewports <br> - Gizmos <br> - Play/Stop mode


## 4. TO DOs
For the handout we will use this example.
![dialogue_project](https://user-images.githubusercontent.com/93879867/230979239-3d7145bb-b907-4927-90d6-da717ad1a915.jpg)


First of all create a module for the dialogue system. That way it will be updated along the game and the own module will manage the GUI buttons for the choices. 

### TODO 1: load dialogue from file

Load the tree with the corresponding ID from file with all its nodes and choices and save it to the **activeTree** Tree. Save as well the **first node** so you can use it to go to the next nodes later.


Load the dialogue tree

```
int DialogueSystem::LoadDialogue(const char* file, int dialogueID)
{
	pugi::xml_parse_result result = dialogues.load_file(file);

	DialogueTree* tree = new DialogueTree(false);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file, result.description());
		return -1;
	}
	else
	{		
		pugi::xml_node pugiNode = dialogues.first_child().first_child();

		for (int i = 0; i <= dialogueID && pugiNode != NULL; i++)
		{
			 if (pugiNode.attribute("ID").as_int() == dialogueID)
			 {
				 tree->treeID = pugiNode.attribute("ID").as_int();		
				 tree->activeNode = LoadNodes(pugiNode, tree);
				 activeTree = tree;	 
				 break;
			 }
			 else
			 {
				 pugiNode = pugiNode.next_sibling("dialogueTree");
			 }
		}
	}

	return dialogueID;
}
``` 


Load tree nodes and save first node. Add all nodes to the list in tree

```
DialogueNode* DialogueSystem::LoadNodes(pugi::xml_node& xml_trees, DialogueTree* tree)
{
	DialogueNode* first_node = new DialogueNode;

	for (pugi::xml_node pugiNode = xml_trees.child("node"); pugiNode != NULL; pugiNode = pugiNode.next_sibling("node"))
	{
		DialogueNode* node = new DialogueNode;

		node->nodeID = pugiNode.attribute("id").as_int();
		node->text = pugiNode.attribute("text").as_string();

		LoadChoices(pugiNode, node);

		tree->nodeList.push_back(node);

		if (node->nodeID == 0) { first_node = node; }	// return the first node to set as the active one
	}

	return first_node;
}
```

Load all choices and add them to the list in node

```
void DialogueSystem::LoadChoices(pugi::xml_node& xml_node, DialogueNode* node)
{
	for (pugi::xml_node choice = xml_node.child("choice"); choice != NULL; choice = choice.next_sibling("choice"))
	{
		DialogueChoice* option = new DialogueChoice;
		option->nextNode = choice.attribute("nextNode").as_int();
		option->text = choice.attribute("option").as_string();
	
		option->eventReturn = choice.attribute("eventReturn").as_int();

		node->choicesList.push_back(option);
	}
}
```

### TODO 2: update dialogue

Update the dialogue if there's an existing tree active and render the text box (and whatever assets you may need).

```
if (activeTree != nullptr)
	{
		//Text box
		iPoint pos = { 0, (app->win->GetHeight() - 353) };
		app->render->DrawTexture(textBox_tex, pos.x, pos.y);

		activeTree->UpdateTree(dt, app->dialogueSystem, pos);
		app->guiManager->Draw();
	}
```

### TODO 3: update dialogue pt.2

Create as many buttons as choices has the active node. It's important to use significant numbers as the ID so later on it can be used to identify the choice chosen. 
(Ex. 0-1-2 -> in the choices array, the ID matches the choice position)

```
GuiButton* button;

	// TODO 3: Create buttons for choices
	for (int i = 0; i < activeNode->choicesList.size(); i++)
	{
		const char* ch_option = activeNode->choicesList[i]->text.GetString();	// SString to const char*	
		int w = activeNode->choicesList[i]->text.Length() * fontSize * 0.5 + 10;
		int h = fontSize + fontSize / 4;
		SDL_Rect bounds = { app->win->GetWidth() - w, pos.y - (h + fontSize) * (i + 1), w, h};

		button = (GuiButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, i, ch_option, bounds, fontSize, mod);
		button->state = GuiControlState::NORMAL;
		listDialogueButtons.Add(button);
	}
```

### TODO 4: handle click events

Set the next node as the active node depending on the clicked choice. In case the decision has to be saved, call the _SaveDialogueState()_ function. 
**Remember to delete the current buttons** to avoid having unnecessary ones that may mess with the IDs.

```
LOG("Event by %d ", control->id);

	// TODO 4: Buttons ID match the choice ID. Use it to access to its attributes
	playerInput = activeTree->activeNode->choicesList[control->id];

	// TODO 4: Save important choices
	if (playerInput->eventReturn == DIALOGUE_SAVE)
	{
		activeTree->activeNode->playerAnswer = control->id;
		SaveDialogueState();
	}

	// Check if last node
	if (playerInput->nextNode != -1)
	{
		// TODO 4: If not last node, set active node next node
		activeTree->activeNode = activeTree->nodeList[playerInput->nextNode];
		activeTree->updateOptions = false;
	} 
	else // If choice leads to the end of the conversation, change active node to last node
	{
		activeTree->activeNode = activeTree->nodeList.at(activeTree->nodeList.size() - 1);
	}
	
	app->guiManager->CleanUp();
```

### TODO 5: render text

The arrray _texts[ ]_ contains the dialogue divided by lines to make it visually more appealing when rendering.

```
// TODO 6: Render dialogue in text box
	size_t lines = activeNode->texts.size();
	for (size_t i = 0; i < lines; i++)
	{
		app->render->TextDraw(activeNode->texts[i].GetString(), pos.x + 100, pos.y + 20 + 50 * i, fontSize, { 255, 255, 255 });
	}
```

Sometimes you may need to trim the text so it fits the textbox...
 
```
int a, b, startIndex = 0;
		for (int j = 0; j <= line.length() / max_chars_line_; j++)	// <= -> in case of decimal, get the round up number 
		{
			a = max_chars_line_ + startIndex;
			b = line.find_first_of(" ", a);	// find first " " (space) from last trimmed to the end. 

			// If we reached the end of the word or the end of the input.
			string temp;
			temp.append(line, startIndex, b - startIndex);	// string text to append, int index start, int size of text to append
			texts.push_back(temp.c_str());
			startIndex = b;
		}
```

Or change a certain word for another based on a variable like the player's name if it can be changed or the amount of currency they have for example. 

```
// TODO 6: Substitute player's name in text and choices if needed
	if (!app->input->playerName.empty())
	{
		activeNode->text.Substitute("%x", app->input->playerName.c_str());
	}
```

### TODO 6: clean ups

Make sure to free everything used to make the dialogue.

```
// TODO 5: Clean Up 
	if (activeTree != nullptr)
	{
		activeTree->nodeList.clear();
		delete activeTree;
		activeTree = nullptr;
	}
```

```
void DialogueTree::CleanUp()
{
	// TODO 5: Clean Up
	for (int j = 0; j < nodeList.size(); j++)
	{
		nodeList[j]->CleanUp();
		delete nodeList[j];
	}
	
	nodeList.clear();
}
```

### TODO 7: save & load

Save

```
bool ret = true;

	// TODO 7: Save player's name and important choices
	pugi::xml_document* saveDoc = new pugi::xml_document();
	pugi::xml_node node = saveDoc->append_child("save_choices");


	pugi::xml_node player = node.append_child("player");

	// save player's name
	player.append_attribute("player_name") = app->input->playerName.c_str();

	// save important choices
	for (size_t i = 0; i < activeTree->nodeList.size(); i++)
	{
		for (int j = 0; j < activeTree->nodeList[i]->choicesList.size(); j++)
		{
			if (activeTree->nodeList[i]->playerAnswer > -1 && activeTree->nodeList[i]->choicesList[j]->eventReturn == 3)
			{
				player = node.append_child("node");
				player.append_attribute("id") = activeTree->nodeList[i]->nodeID;
				player.append_attribute("answer") = activeTree->nodeList[i]->playerAnswer;
				player.append_attribute("text") = activeTree->nodeList[i]->choicesList[activeTree->nodeList[i]->playerAnswer]->text.GetString();
				break;
			}
		}
	}

	ret = saveDoc->save_file("save_dialogue.xml");

	return ret;
```

Load

```
pugi::xml_document gameStateFile;
	pugi::xml_parse_result result = gameStateFile.load_file("save_dialogue.xml");
	pugi::xml_node node = gameStateFile.child("save_choices");
	
	bool ret = true;

	if (result == NULL)
	{
		LOG("Could not load xml file save_dialogue.xml. pugi error: %s", result.description());
		ret = false;
	}
	else
	{
		// TODO 7: Load player's name and important choices from xml
		string temp = node.child("player").attribute("player_name").as_string();
		app->input->playerName = temp.c_str();
		app->input->nameEntered = true;

		for (size_t i = 0; i < activeTree->nodeList.size() && !node.child("node").empty(); i++)
		{
			for (int j = 0; j < activeTree->nodeList[i]->choicesList.size(); j++)
			{
				if (activeTree->nodeList[i]->choicesList[j]->eventReturn == 3)
				{
					activeTree->nodeList[i]->nodeID = node.child("node").attribute("id").as_int();
					activeTree->nodeList[i]->playerAnswer = node.child("node").attribute("answer").as_int();
				}
			}
		}
	}

	return ret;
```

> Have in mind that those functions will only be called when specified and NOT every time the game is saved like the other modules. 

## 5. References
- [defining-dialogue-systems](https://www.gamedeveloper.com/design/defining-dialogue-systems)

- [successful-in-game-dialogue-should-carry-players-forward](https://www.polygon.com/2014/3/17/5519270/successful-in-game-dialogue-should-carry-players-forward#:~:text=%22Conversations%20are%20a%20hallmark%20of,to%20new%20characters%20and%20locations)

- [video-game-dialogue-writing](https://gamedesignlounge.com/video-game-dialogue-writing/)

- [Realistic Dialogue Engine for Video Games](https://ir.lib.uwo.ca/cgi/viewcontent.cgi?referer=https://www.google.co.uk/&httpsredir=1&article=4141&context=etd)

- [A Summary of Research on Intelligent Dialogue Systems](https://iopscience.iop.org/article/10.1088/1742-6596/1651/1/012020/pdf#:~:text=Classification%20of%20dialogue%20systems&text=According%20to%20the%20purpose%20of,and%20small%2Dtalk%20dialogue%20system)

Other projects
- [Marckitus](https://github.com/Marckitus/Dialog-Systems)

- [Xymaru](https://github.com/Xymaru/DialogSystem)

- [Bookfan97](https://github.com/Bookfan97/DialogSystem)
