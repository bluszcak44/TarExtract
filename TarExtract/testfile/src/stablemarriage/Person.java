/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package stablemarriage;

import java.awt.Image;

/**
 *
 * @author wolfgang bode
 */
public class Person  implements PersonInterface {
    private java.awt.Image image;
    private String name;
    private PersonInterface spouse;
    private java.util.ArrayList<PersonInterface> preferences = new java.util.ArrayList<PersonInterface>();

    public Person( String name, Image image, PersonInterface spouse, java.util.ArrayList<PersonInterface> preferences) {
        this.name = name;
        this.image = image;
        this.spouse = spouse;
        this.preferences.addAll(preferences);
    }

    public Person(String name, Image image, PersonInterface spouse, PersonInterface [] preferences) {
        this.name = name;
        this.image = image;
        this.spouse = spouse;
        for (PersonInterface person : preferences) {
            this.preferences.add(person);
        }
    }

    public Person(String name, Image image, PersonInterface spouse) {
        this.name = name;
        this.image = image;
        this.spouse = spouse;
    }

    public void setPreferences(PersonInterface [] preferences) {
        this.preferences.clear();
        for (PersonInterface person : preferences) {
            this.preferences.add(person);
        }
    }
    public Image getImage() {
            return image;
    }


    public String getName() {
        return name;
    }

    public PersonInterface getSpouse() {
        return spouse;
    }

    public boolean isMarried() {
        return spouse != null;
    }

    public boolean isSingle() {
        return spouse == null;
    }

    public boolean prefers(PersonInterface one, PersonInterface two) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void setImage(Image image) {
        this.image = image;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setSpouse(PersonInterface spouse) {
        this.spouse = spouse;
    }

}
